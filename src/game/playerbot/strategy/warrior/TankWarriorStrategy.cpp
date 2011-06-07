#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "WarriorTriggers.h"
#include "WarriorMultipliers.h"
#include "TankWarriorStrategy.h"
#include "WarriorActions.h"

using namespace ai;

NextAction** TankWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("melee", 10.0f), NULL);
}

void TankWarriorStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new MediumRageAvailable(ai), 
        NextAction::array(0, new NextAction("heroic strike", 1.3f), NULL)));

	triggers.push_back(new TriggerNode(
		new MediumRageAvailable(ai), 
		NextAction::array(0, new NextAction("shield slam", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LightRageAvailable(ai), 
        NextAction::array(0, new NextAction("devastate", 1.4f), NULL)));

    triggers.push_back(new TriggerNode(
        new DisarmDebuffTrigger(ai), 
        NextAction::array(0, new NextAction("disarm", 1.2f), NULL)));

    triggers.push_back(new TriggerNode(
        new LoseAggroTrigger(ai), 
        NextAction::array(0, new NextAction("taunt", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new MediumHealthTrigger(ai), 
        NextAction::array(0, new NextAction("shield wall", 50.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new CriticalHealthTrigger(ai), 
		NextAction::array(0, new NextAction("last stand", 91.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new MediumAoeTrigger(ai), 
		NextAction::array(0, new NextAction("shockwave", 24.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new SnareTargetTrigger(ai, "concussion blow"), 
		NextAction::array(0, new NextAction("concussion blow", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new SwordAndBoardTrigger(ai), 
        NextAction::array(0, new NextAction("shield slam", 40.0f), NULL)));
}

void TankWarriorStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{

}

ActionNode* TankWarriorStrategy::createAction(const char* name)
{
    if (!strcmp("melee", name)) 
    {
        return new ActionNode (new MeleeAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("defensive stance"), new NextAction("reach melee"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("shield wall", name)) 
    {
        return new ActionNode (new CastShieldWallAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shield block"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("rend", name)) 
    {
        return new ActionNode (new CastRendAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("defensive stance"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("taunt", name)) 
    {
        return new ActionNode (new CastTauntAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("revenge", name)) 
    {
        return new ActionNode (new CastRevengeAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("slam"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("slam", name)) 
    {
        return new ActionNode (new CastSlamAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("shield slam", name)) 
	{
		return new ActionNode (new CastShieldSlamAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("disarm", name)) 
    {
        return new ActionNode (new CastDisarmAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("sunder armor", name)) 
    {
        return new ActionNode (new CastSunderArmorAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("devastate", name)) 
    {
        return new ActionNode (new CastDevastateAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("sunder armor"), NULL), 
            /*C*/ NextAction::array(0, new NextAction("revenge", 10.0f), NULL));
    }
    else if (!strcmp("shield bash", name)) 
    {
        return new ActionNode (new CastShieldBashAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("intimidating shout", name)) 
    {
        return new ActionNode (new CastIntimidatingShoutAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("last stand", name)) 
	{
		return new ActionNode (new CastLastStandAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
	else if (!strcmp("shockwave", name)) 
	{
		return new ActionNode (new CastShockwaveAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
	else if (!strcmp("concussion blow", name)) 
	{
		return new ActionNode (new CastConcussionBlowAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else return GenericWarriorStrategy::createAction(name);
}