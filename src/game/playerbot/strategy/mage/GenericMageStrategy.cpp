#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "MageTriggers.h"
#include "MageMultipliers.h"
#include "GenericMageStrategy.h"
#include "MageActions.h"

using namespace ai;

void GenericMageStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new NeedCureTrigger(ai, "remove curse", DISPEL_CURSE),
        NextAction::array(0, new NextAction("remove curse", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new PartyMemberNeedCureTrigger(ai, "remove curse", DISPEL_CURSE),
        NextAction::array(0, new NextAction("remove curse on party", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new EnemyTooCloseTrigger(ai),
        NextAction::array(0, new NextAction("frost nova", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CounterspellInterruptSpellTrigger(ai),
        NextAction::array(0, new NextAction("counterspell", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new CriticalHealthTrigger(ai),
		NextAction::array(0, new NextAction("ice block", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HasCcTargetTrigger(ai, "polymorph"),
        NextAction::array(0, new NextAction("polymorph", 30.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new TargetAuraDispelTrigger(ai, "spellsteal", DISPEL_MAGIC),
		NextAction::array(0, new NextAction("spellsteal", 40.0f), NULL)));
}


ActionNode* GenericMageStrategy::createAction(const char* name)
{
    if (!strcmp("frostbolt", name))
    {
        return new ActionNode (new CastFrostboltAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shoot"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("fireball", name))
    {
        return new ActionNode (new CastFireballAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("pyroblast", name))
    {
        return new ActionNode (new CastPyroblastAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("flamestrike", name))
    {
        return new ActionNode (new CastFlamestrikeAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("fire blast", name))
    {
        return new ActionNode (new CastFireBlastAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("scorch"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("scorch", name))
    {
        return new ActionNode (new CastScorchAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shoot"), NULL),
            /*C*/ NULL);
    }
	else if (!strcmp("reach spell", name))
	{
		return new ActionNode (new ReachSpellAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("flee", name))
	{
		return new ActionNode (new FleeAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("frost nova", name))
    {
        return new ActionNode (new CastFrostNovaAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("flee"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee"), NULL));
    }
    else if (!strcmp("counterspell", name))
    {
        return new ActionNode (new CastCounterspellAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("remove curse", name))
    {
        return new ActionNode (new CastRemoveCurseAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("remove curse on party", name))
    {
        return new ActionNode (new CastRemoveCurseOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("icy veins", name) || !strcmp("boost", name))
    {
        return new ActionNode (new CastIcyVeinsAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("combustion", name) || !strcmp("boost", name))
    {
        return new ActionNode (new CastCombustionAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("ice block", name))
	{
		return new ActionNode (new CastIceBlockAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("polymorph", name))
    {
        return new ActionNode (new CastPolymorphAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("spellsteal", name))
	{
		return new ActionNode (new CastSpellstealAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else return NULL;
}
