#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "PaladinTriggers.h"
#include "PaladinMultipliers.h"
#include "TankPaladinStrategy.h"
#include "PaladinActions.h"

using namespace ai;

NextAction** TankPaladinStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("melee", 10.0f), NULL);
}

void TankPaladinStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericPaladinStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new JudgementOfLightTrigger(ai), 
        NextAction::array(0, new NextAction("judgement of light", 1.2f), NULL)));

    triggers.push_back(new TriggerNode(
        new RighteousFuryTrigger(ai), 
        NextAction::array(0, new NextAction("righteous fury", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LightAoeTrigger(ai), 
        NextAction::array(0, new NextAction("hammer of the righteous", 26.0f), new NextAction("avenger's shield", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new MediumAoeTrigger(ai), 
        NextAction::array(0, new NextAction("consecration", 27.0f), new NextAction("holy wrath", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LoseAggroTrigger(ai), 
        NextAction::array(0, new NextAction("hand of reckoning", 16.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new HolyShieldTrigger(ai), 
		NextAction::array(0, new NextAction("holy shield", 18.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new BlessingTrigger(ai), 
        NextAction::array(0, new NextAction("blessing of sanctuary", 21.0f), NULL)));
}

void TankPaladinStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{

}

ActionNode* TankPaladinStrategy::createAction(const char* name)
{
    if (!strcmp("righteous fury", name)) 
    {
        return new ActionNode (new CastRighteousFuryAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("hand of reckoning", name)) 
    {
        return new ActionNode (new CastHandOfReckoningAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("judgement of justice"), NULL), 
            /*C*/ NULL);
    }
    else if (!strcmp("avenger's shield", name)) 
    {
        return new ActionNode (new CastAvengersShieldAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("exorcism", name)) 
	{
		return new ActionNode (new CastExorcismAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
	else if (!strcmp("holy shield", name)) 
	{
		return new ActionNode (new CastHolyShieldAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
	else if (!strcmp("hammer of the righteous", name)) 
	{
		return new ActionNode (new CastHammerOfTheRighteousAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("blessing of sanctuary", name)) 
    {
        return new ActionNode (new CastBlessingOfSanctuaryAction(ai),  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("blessing of kings"), NULL), 
            /*C*/ NULL);
    }
    else return GenericPaladinStrategy::createAction(name);
}