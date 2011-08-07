#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "AttackersValue.h"

#include "../../../GridNotifiers.h"
#include "../../../GridNotifiersImpl.h"
#include "../../../CellImpl.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> AttackersValue::Calculate()
{
    set<Unit*> targets;

    AddAttackersOf(bot, targets);

    Group* group = bot->GetGroup();
    if (group)
        AddAttackersOf(group, targets);

    RemoveNonThreating(targets);

    list<ObjectGuid> result;
	for (set<Unit*>::iterator i = targets.begin(); i != targets.end(); i++)
		result.push_back((*i)->GetObjectGuid());
	return result;
}

void AttackersValue::AddAttackersOf(Group* group, set<Unit*>& targets)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player *member = sObjectMgr.GetPlayer(itr->guid);
        if (!member || !member->isAlive() || member == bot || member->GetMapId() != master->GetMapId())
            continue;

        AddAttackersOf(member, targets);
    }
}

void AttackersValue::AddAttackersOf(Player* player, set<Unit*>& targets)
{
	list<Unit*> units;
	MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(player, player, sPlayerbotAIConfig.sightDistance);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(units, u_check);
    Cell::VisitAllObjects(player, searcher, sPlayerbotAIConfig.sightDistance);
	for (list<Unit*>::iterator i = units.begin(); i != units.end(); i++)
		targets.insert(*i);
}

void AttackersValue::RemoveNonThreating(set<Unit*>& targets)
{
    for(set<Unit *>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if(!bot->IsWithinLOSInMap(*tIter) || !hasRealThreat(*tIter))
        {
            set<Unit *>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }
}

bool AttackersValue::hasRealThreat(Unit *attacker)
{
    return attacker &&
        !attacker->isDead() &&
        !attacker->IsPolymorphed() &&
        !attacker->isFrozen() &&
        !attacker->isInRoots() &&
        !attacker->IsFriendlyTo(master) &&
        attacker->getThreatManager().getCurrentVictim();
}
