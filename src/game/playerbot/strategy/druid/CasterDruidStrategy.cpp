#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "DruidMultipliers.h"
#include "CasterDruidStrategy.h"

using namespace ai;

class CasterDruidStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    CasterDruidStrategyActionNodeFactory()
    {
        creators["faerie fire"] = &faerie_fire;
        creators["hibernate"] = &hibernate;
        creators["entangling roots"] = &entangling_roots;
        creators["entangling roots on cc"] = &entangling_roots_on_cc;
        creators["wrath"] = &wrath;
        creators["starfall"] = &starfall;
        creators["insect swarm"] = &insect_swarm;
        creators["moonfire"] = &moonfire;
        creators["starfire"] = &starfire;
        creators["nature's grasp"] = &natures_grasp;
    }
private:
    static ActionNode* faerie_fire(PlayerbotAI* ai)
    {
        return new ActionNode ("faerie fire",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* hibernate(PlayerbotAI* ai)
    {
        return new ActionNode ("hibernate",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NextAction::array(0, new NextAction("entangling roots"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee", 49.0f), NULL));
    }
    static ActionNode* entangling_roots(PlayerbotAI* ai)
    {
        return new ActionNode ("entangling roots",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NextAction::array(0, new NextAction("flee", 49.0f), NULL));
    }
    static ActionNode* entangling_roots_on_cc(PlayerbotAI* ai)
    {
        return new ActionNode ("entangling roots on cc",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* wrath(PlayerbotAI* ai)
    {
        return new ActionNode ("wrath",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* starfall(PlayerbotAI* ai)
    {
        return new ActionNode ("starfall",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NextAction::array(0, new NextAction("hurricane"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* insect_swarm(PlayerbotAI* ai)
    {
        return new ActionNode ("insect swarm",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* moonfire(PlayerbotAI* ai)
    {
        return new ActionNode ("moonfire",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* starfire(PlayerbotAI* ai)
    {
        return new ActionNode ("starfire",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
    static ActionNode* natures_grasp(PlayerbotAI* ai)
    {
        return new ActionNode ("nature's grasp",
            /*P*/ NextAction::array(0, new NextAction("moonkin form"), NULL),
            /*A*/ NULL,
            /*C*/ NULL);
    }
};

CasterDruidStrategy::CasterDruidStrategy(PlayerbotAI* ai) : GenericDruidStrategy(ai)
{
    actionNodeFactories.Add(new CasterDruidStrategyActionNodeFactory());
}

NextAction** CasterDruidStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("starfire", 11.0f), new NextAction("wrath", 10.0f), NULL);
}

void CasterDruidStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericDruidStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "medium health",
        NextAction::array(0, new NextAction("regrowth", 49.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member medium health",
        NextAction::array(0, new NextAction("regrowth on party", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "almost full health",
        NextAction::array(0, new NextAction("rejuvenation",49.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "party member almost full health",
        NextAction::array(0, new NextAction("rejuvenation on party", 49.0f), NULL)));


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
