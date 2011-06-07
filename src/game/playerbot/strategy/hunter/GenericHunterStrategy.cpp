#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "../GenericTriggers.h"
#include "../GenericActions.h"
#include "HunterTriggers.h"
#include "GenericHunterStrategy.h"
#include "HunterActions.h"
#include "HunterActionFactory.h"

using namespace ai;

ActionFactory* GenericHunterStrategy::createActionFactory() 
{
    return new HunterActionFactory(ai); 
}

void GenericHunterStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new EnemyTooCloseTrigger(ai), 
        NextAction::array(0, new NextAction("flee", 50.0f), new NextAction("concussive shot", 40.0f), NULL)));    
    
    triggers.push_back(new TriggerNode(
        new HuntersPetDeadTrigger(ai), 
        NextAction::array(0, new NextAction("revive pet", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HuntersPetLowHealthTrigger(ai), 
        NextAction::array(0, new NextAction("mend pet", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new RapidFireTrigger(ai),
        NextAction::array(0, new NextAction("rapid fire", 55.0f), NULL)));
}

ActionNode* GenericHunterStrategy::createAction(const char* name)
{
    if (!strcmp("call pet", name)) 
    {
        return new ActionNode (new CastCallPetAction(ai),  
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
    else if (!strcmp("mend pet", name)) 
    {
        return new ActionNode (new CastMendPetAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("revive pet", name)) 
    {
        return new ActionNode (new CastRevivePetAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("rapid fire", name) || !strcmp("boost", name)) 
    {
        return new ActionNode (new CastRapidFireAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("readyness"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("readyness", name)) 
    {
        return new ActionNode (new CastReadynessAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("aspect of the hawk", name)) 
    {
        return new ActionNode (new CastAspectOfTheHawkAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("aspect of the wild", name)) 
	{
		return new ActionNode (new CastAspectOfTheWildAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("aspect of the viper", name)) 
    {
        return new ActionNode (new CastAspectOfTheViperAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else return NULL;
}
