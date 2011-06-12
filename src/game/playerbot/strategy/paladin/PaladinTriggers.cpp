#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "PaladinTriggers.h"
#include "PaladinActions.h"

using namespace ai;

bool SealTrigger::IsActive() 
{
	Unit* target = GetTarget();
	return AI_VALUE2(uint8, "mana", "self target") > 30 && 
		!spellManager->HasAura("seal of justice", target) &&
        !spellManager->HasAura("seal of command", target) &&
		!spellManager->HasAura("seal of righteousness", target) &&
		!spellManager->HasAura("seal of light", target) &&
		!spellManager->HasAura("seal of wisdom", target);
}

bool CrusaderAuraTrigger::IsActive() 
{
	Unit* target = GetTarget();
	return AI_VALUE(bool, "mounted") && !spellManager->HasAura("crusader aura", target);
}