#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "WarlockTriggers.h"
#include "WarlockActions.h"

using namespace ai;

bool DemonArmorTrigger::IsActive() 
{
	Unit* target = GetTarget();
	return AI_VALUE2(uint8, "mana", "self target") > 30 && 
		!spellManager->HasAura("demon skin", target) &&
		!spellManager->HasAura("demon armor", target);
}

bool SpellstoneTrigger::IsActive() 
{
    return BuffTrigger::IsActive() && ai->GetInventoryManager()->GetItemCount(getName()) > 0;
}