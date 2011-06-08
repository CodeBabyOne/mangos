#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "MageMultipliers.h"
#include "FrostMageStrategy.h"

using namespace ai;

NextAction** FrostMageStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("frostbolt", 1.0f), NULL);
}

void FrostMageStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericMageStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "icy veins",
        NextAction::array(0, new NextAction("icy veins", 50.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"high aoe",
		NextAction::array(0, new NextAction("blizzard", 40.0f), NULL)));
}

void FrostMageStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{
    GenericMageStrategy::InitMultipliers(multipliers);
}

ActionNode* FrostMageStrategy::createAction(const char* name)
{
    ActionNode* node = GenericMageStrategy::createAction(name);
    if (node)
        return node;

    if (!strcmp("frostbolt", name)) 
    {
        return new ActionNode ("frostbolt",  
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shoot"), NULL), 
            /*C*/ NULL);
    }
	else if (!strcmp("blizzard", name)) 
	{
		return new ActionNode ("blizzard",  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else if (!strcmp("frost nova", name)) 
    {
        return new ActionNode ("frost nova",  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NextAction::array(0, new NextAction("flee"), new NextAction("frostbolt"), NULL));
    }

    else return GenericMageStrategy::createAction(name);
}