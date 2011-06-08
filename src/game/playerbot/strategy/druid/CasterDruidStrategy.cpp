#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "DruidTriggers.h"
#include "DruidMultipliers.h"
#include "DruidActions.h"
#include "CasterDruidStrategy.h"

using namespace ai;


NextAction** CasterDruidStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("starfire", 11.0f), new NextAction("wrath", 10.0f), NULL);
}

void CasterDruidStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericDruidStrategy::InitTriggers(triggers);

	triggers.push_back(new TriggerNode(
		"insect swarm",
		NextAction::array(0, new NextAction("insect swarm", 15.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"moonfire",
		NextAction::array(0, new NextAction("moonfire", 14.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"faerie fire",
		NextAction::array(0, new NextAction("faerie fire", 16.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"nature's grasp",
		NextAction::array(0, new NextAction("nature's grasp", 20.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"high aoe",
		NextAction::array(0, new NextAction("starfall", 17.0f), NULL)));
}

void CasterDruidStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{

}


ActionNode* CasterDruidStrategy::createAction(const char* name)
{
    if (!strcmp("faerie fire", name))
    {
        return new ActionNode (new CastFaerieFireAction(ai),
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
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
    else if (!strcmp("moonkin form", name))
    {
        return new ActionNode (new CastMoonkinFormAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("hibernate", name))
	{
		return new ActionNode (new CastHibernateAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NextAction::array(0, new NextAction("entangling roots"), NULL),
			/*C*/ NextAction::array(0, new NextAction("flee", 49.0f), NULL));
	}
	else if (!strcmp("entangling roots", name))
	{
		return new ActionNode (new CastEntanglingRootsAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NextAction::array(0, new NextAction("flee", 49.0f), NULL));
	}
    else if (!strcmp("entangling roots on cc", name))
    {
        return new ActionNode (new CastEntanglingRootsCcAction(ai),
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("wrath", name))
	{
		return new ActionNode (new CastWrathAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("starfall", name))
	{
		return new ActionNode (new CastStarfallAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("insect swarm", name))
	{
		return new ActionNode (new CastInsectSwarmAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("moonfire", name))
	{
		return new ActionNode (new CastMoonfireAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("starfire", name))
	{
		return new ActionNode (new CastStarfireAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("nature's grasp", name))
	{
		return new ActionNode (new CastNaturesGraspAction(ai),
			/*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else return GenericDruidStrategy::createAction(name);
}
