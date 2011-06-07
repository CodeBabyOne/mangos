#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "DruidTriggers.h"
#include "GenericDruidStrategy.h"
#include "DruidActions.h"
#include "DruidActionFactory.h"

using namespace ai;

ActionFactory* GenericDruidStrategy::createActionFactory() 
{
    return new DruidActionFactory(ai); 
}

void GenericDruidStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

	triggers.push_back(new TriggerNode(
		new MediumHealthTrigger(ai),
		NextAction::array(0, new NextAction("regrowth", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LowHealthTrigger(ai),
        NextAction::array(0, new NextAction("rejuvenation", 70.0f), new NextAction("healing touch", 70.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new PartyMemberMediumHealthTrigger(ai),
		NextAction::array(0, new NextAction("regrowth on party", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new PartyMemberLowHealthTrigger(ai),
        NextAction::array(0, new NextAction("rejuvenation on party", 70.0f), new NextAction("healing touch on party", 70.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new NeedCureTrigger(ai, "cure poison", DISPEL_POISON),
        NextAction::array(0, new NextAction("abolish poison", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new PartyMemberNeedCureTrigger(ai, "cure poison", DISPEL_POISON),
        NextAction::array(0, new NextAction("abolish poison on party", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new PartyMemberDeadTrigger(ai),
		NextAction::array(0, new NextAction("rebirth", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HasCcTargetTrigger(ai, "entangling roots"),
        NextAction::array(0, new NextAction("entangling roots on cc", 31.0f), NULL)));
}

ActionNode* GenericDruidStrategy::createAction(const char* name)
{
    if (!strcmp("melee", name)) 
    {
        return new ActionNode (new MeleeAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("reach melee"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("caster form", name)) 
    {
        return new ActionNode (new CastCasterFormAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("gift of the naaru", name)) 
    {
        return new ActionNode (new CastGiftOfTheNaaruAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("regrowth"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("regrowth", name)) 
    {
        return new ActionNode (new CastRegrowthAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NextAction::array(0, new NextAction("melee", 10.0f), NULL));
    }
    else if (!strcmp("rejuvenation", name)) 
    {
        return new ActionNode (new CastRejuvenationAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("healing touch", name)) 
    {
        return new ActionNode (new CastHealingTouchAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("regrowth on party", name)) 
    {
        return new ActionNode (new CastRegrowthOnPartyAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NextAction::array(0, new NextAction("melee", 10.0f), NULL));
    }
    else if (!strcmp("rejuvenation on party", name)) 
    {
        return new ActionNode (new CastRejuvenationOnPartyAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("healing touch on party", name)) 
    {
        return new ActionNode (new CastHealingTouchOnPartyAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("cure poison", name)) 
    {
        return new ActionNode (new CastCurePoisonAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("cure poison on party", name)) 
    {
        return new ActionNode (new CastCurePoisonOnPartyAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("abolish poison", name)) 
    {
        return new ActionNode (new CastAbolishPoisonAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("abolish poison on party", name)) 
    {
        return new ActionNode (new CastAbolishPoisonOnPartyAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("rebirth", name)) 
	{
		return new ActionNode (new CastRebirthAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("entangling roots on cc", name)) 
    {
        return new ActionNode (new CastEntanglingRootsCcAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }

    else return NULL;
}
