#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "UseFoodStrategy.h"

using namespace ai;

void UseFoodStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    Strategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("stay", 9.0f), new NextAction("food", 9.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("stay", 9.0f), new NextAction("drink", 9.0f), NULL)));
}
