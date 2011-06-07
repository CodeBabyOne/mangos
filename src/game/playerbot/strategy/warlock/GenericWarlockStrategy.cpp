#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "WarlockTriggers.h"
#include "WarlockMultipliers.h"
#include "GenericWarlockStrategy.h"
#include "WarlockActions.h"

using namespace ai;

NextAction** GenericWarlockStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("shoot", 10.0f), NULL);
}

void GenericWarlockStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);
    
    triggers.push_back(new TriggerNode(
        new CorruptionTrigger(ai),
        NextAction::array(0, new NextAction("corruption", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CurseOfAgonyTrigger(ai),
        NextAction::array(0, new NextAction("curse of agony", 11.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new MediumHealthTrigger(ai),
        NextAction::array(0, new NextAction("drain life", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new TargetCriticalHealthTrigger(ai),
		NextAction::array(0, new NextAction("drain soul", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new BanishTrigger(ai),
        NextAction::array(0, new NextAction("banish", 21.0f), NULL)));
}


ActionNode* GenericWarlockStrategy::createAction(const char* name)
{
    if (!strcmp("immolate", name)) 
    {
        return new ActionNode (new CastImmolateAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
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
    else if (!strcmp("corruption", name)) 
    {
        return new ActionNode (new CastCorruptionAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("summon voidwalker", name)) 
    {
        return new ActionNode (new CastSummonVoidwalkerAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("drain soul"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("curse of agony", name)) 
    {
        return new ActionNode (new CastCurseOfAgonyAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("shadow bolt", name)) 
    {
        return new ActionNode (new CastShadowBoltAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("drain soul", name)) 
    {
        return new ActionNode (new CastDrainSoulAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("drain mana", name)) 
    {
        return new ActionNode (new CastDrainManaAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("drain life", name)) 
    {
        return new ActionNode (new CastDrainLifeAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("banish", name)) 
    {
        return new ActionNode (new CastBanishAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else return NULL;
}


