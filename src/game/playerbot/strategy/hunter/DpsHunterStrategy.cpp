#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "../GenericTriggers.h"
#include "../GenericActions.h"

#include "HunterTriggers.h"
#include "HunterMultipliers.h"
#include "DpsHunterStrategy.h"
#include "HunterActions.h"

using namespace ai;

NextAction** DpsHunterStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("explosive shot", 11.0f), new NextAction("auto shot", 10.0f), NULL);
}

void DpsHunterStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new HunterAspectOfTheViperTrigger(ai), 
        NextAction::array(0, new NextAction("aspect of the viper", 91.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new DebuffTrigger(ai, "black arrow"), 
        NextAction::array(0, new NextAction("black arrow", 51.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HunterNoStingsActiveTrigger(ai), 
        NextAction::array(0, new NextAction("serpent sting", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LowManaTrigger(ai), 
        NextAction::array(0, new NextAction("viper sting", 51.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new NoPetTrigger(ai), 
        NextAction::array(0, new NextAction("call pet", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HuntersPetDeadTrigger(ai), 
        NextAction::array(0, new NextAction("revive pet", 5.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HuntersPetLowHealthTrigger(ai), 
        NextAction::array(0, new NextAction("mend pet", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new DebuffTrigger(ai, "hunter's mark"), 
        NextAction::array(0, new NextAction("hunter's mark", 52.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HasCcTargetTrigger(ai, "freezing trap"), 
        NextAction::array(0, new NextAction("freezing trap", 83.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new MediumAoeTrigger(ai), 
        NextAction::array(0, new NextAction("multi-shot", 20.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new HighAoeTrigger(ai), 
		NextAction::array(0, new NextAction("volley", 20.0f), NULL)));
}

void DpsHunterStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{
    GenericHunterStrategy::InitMultipliers(multipliers);
}

ActionNode* DpsHunterStrategy::createAction(const char* name)
{
    ActionNode* node = GenericHunterStrategy::createAction(name);
    if (node)
        return node;

    if (!strcmp("auto shot", name)) 
    {
        return new ActionNode (new CastAutoShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("aimed shot", name)) 
    {
        return new ActionNode (new CastAimedShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("arcane shot", 10.0f), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("explosive shot", name)) 
    {
        return new ActionNode (new CastExplosiveShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("aimed shot"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("arcane shot", name)) 
    {
        return new ActionNode (new CastArcaneShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("concussive shot", name)) 
    {
        return new ActionNode (new CastConcussiveShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NextAction::array(0, new NextAction("wyvern sting", 11.0f), NULL));
    }
    else if (!strcmp("distracting shot", name)) 
    {
        return new ActionNode (new CastDistractingShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("multi-shot", name)) 
    {
        return new ActionNode (new CastMultiShotAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("volley", name)) 
	{
		return new ActionNode (new CastVolleyAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("serpent sting", name)) 
    {
        return new ActionNode (new CastSerpentStingAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("wyvern sting", name)) 
    {
        return new ActionNode (new CastWyvernStingAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("viper sting", name)) 
    {
        return new ActionNode (new CastViperStingAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("scorpid sting", name)) 
    {
        return new ActionNode (new CastScorpidStingAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("hunter's mark", name)) 
    {
        return new ActionNode (new CastHuntersMarkAction(ai),  
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
    else if (!strcmp("black arrow", name)) 
    {
        return new ActionNode (new CastBlackArrow(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("freezing trap", name)) 
    {
        return new ActionNode (new CastFreezingTrap(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else return NULL;
}

