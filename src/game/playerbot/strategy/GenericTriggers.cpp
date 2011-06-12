#include "../../pchdef.h"
#include "../playerbot.h"
#include "GenericTriggers.h"

using namespace ai;

bool LowManaTrigger::IsActive()
{
	Unit* target = targetManager->GetSelf();
	return statsManager->HasMana(target) && statsManager->GetManaPercent(target) < LOW_HEALTH_PERCENT;
}


bool RageAvailable::IsActive()
{
    return statsManager->GetRage(targetManager->GetSelf()) >= amount;
}

bool EnergyAvailable::IsActive()
{
	return statsManager->GetEnergy(targetManager->GetSelf()) >= amount;
}

bool ComboPointsAvailableTrigger::IsActive()
{
    return statsManager->GetComboPoints((Player*)targetManager->GetSelf()) >= amount;
}

bool LoseAggroTrigger::IsActive()
{
    return !statsManager->HasAggro(targetManager->GetCurrentTarget());
}

bool PanicTrigger::IsActive()
{
    return ai->GetStatsManager()->GetHealthPercent(ai->GetTargetManager()->GetSelf()) < 25 &&
		(!statsManager->HasMana(targetManager->GetSelf()) || statsManager->GetManaPercent(targetManager->GetSelf()) < 25);
}

Unit* BuffTrigger::GetTarget()
{
	return targetManager->GetSelf();
}

bool BuffTrigger::IsActive()
{
    Unit* target = GetTarget();
    Unit* self = targetManager->GetSelf();
	return SpellTrigger::IsActive() &&
		!spellManager->HasAura(spell, target) &&
		(!statsManager->HasMana(self) || statsManager->GetManaPercent(self) > 40);
}

Unit* BuffOnPartyTrigger::GetTarget()
{
	return targetManager->GetPartyMemberWithoutAura(spell);
}

bool NoAttackersTrigger::IsActive()
{
    return !targetManager->GetCurrentTarget() && statsManager->GetAttackerCount() > 0;
}

bool NoTargetTrigger::IsActive()
{
	return !targetManager->GetCurrentTarget();
}

bool MyAttackerCountTrigger::IsActive()
{
    return statsManager->GetMyAttackerCount() >= amount;
}

bool AoeTrigger::IsActive()
{
    return statsManager->GetAttackerCount(targetManager->GetCurrentTarget(), range) >= amount;
}

Unit* DebuffTrigger::GetTarget()
{
	return targetManager->GetCurrentTarget();
}

bool DebuffTrigger::IsActive()
{
	return BuffTrigger::IsActive() && statsManager->GetHealthPercent(GetTarget()) > 25;
}

Unit* SpellTrigger::GetTarget()
{
	return targetManager->GetCurrentTarget();
}

bool SpellTrigger::IsActive()
{
	return GetTarget();
}

bool SpellCanBeCastTrigger::IsActive()
{
	Unit* target = GetTarget();
	return target && spellManager->CanCastSpell(spell, target);
}

bool LootAvailableTrigger::IsActive()
{
    return ai->GetInventoryManager()->CanLoot();
}

bool RandomTrigger::IsActive()
{
    int vl  = rand() % probability;
    return vl == 0;
}

bool AndTrigger::IsActive()
{
    return ls->IsActive() && rs->IsActive();
}

const char* AndTrigger::getName()
{
    std::string name(ls->getName());
    name = name + " and ";
    name = name + rs->getName();
    return name.c_str();
}

bool BoostTrigger::IsActive()
{
	return BuffTrigger::IsActive() && statsManager->GetBalancePercent() <= balance;
}

bool SnareTargetTrigger::IsActive()
{
	Unit* target = GetTarget();
	return DebuffTrigger::IsActive() && moveManager->IsMoving(target) && !spellManager->HasAura(spell, target);
}

bool ItemCountTrigger::IsActive()
{
	return ai->GetInventoryManager()->GetItemCount(item) < count;
}

Unit* InterruptSpellTrigger::GetTarget()
{
	return targetManager->GetCurrentTarget();
}

bool InterruptSpellTrigger::IsActive()
{
	return SpellTrigger::IsActive() && spellManager->IsSpellCasting(GetTarget());
}

Unit* HasAuraTrigger::GetTarget()
{
	return targetManager->GetSelf();
}

bool HasAuraTrigger::IsActive()
{
	return spellManager->HasAura(spell, GetTarget());
}

bool TankAoeTrigger::IsActive()
{
    Unit* target = targetManager->GetCurrentTarget();
    return statsManager->GetAttackerCount() > 0 &&
        (!target || target->getVictim() == targetManager->GetSelf());
}

bool IsBehindTargetTrigger::IsActive()
{
    Unit* target = targetManager->GetCurrentTarget();
    return target && moveManager->IsBehind(target);
}

bool HasCcTargetTrigger::IsActive()
{
    return statsManager->GetAttackerCount() > 2 && targetManager->FindCcTarget(getName()) && !targetManager->GetCurrentCcTarget(getName());
}

bool NoMovementTrigger::IsActive()
{
	return !moveManager->IsMoving(targetManager->GetSelf());
}
