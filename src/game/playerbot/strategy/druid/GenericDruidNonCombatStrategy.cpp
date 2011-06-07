#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "DruidTriggers.h"
#include "DruidMultipliers.h"
#include "GenericDruidNonCombatStrategy.h"
#include "DruidActions.h"

using namespace ai;

void GenericDruidNonCombatStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericNonCombatStrategy::InitTriggers(triggers);
        
    triggers.push_back(new TriggerNode(
        new MarkOfTheWildTrigger(ai),
        NextAction::array(0, new NextAction("mark of the wild", 1.0f), NULL)));
    
    triggers.push_back(new TriggerNode(
        new MarkOfTheWildOnPartyTrigger(ai),
        NextAction::array(0, new NextAction("mark of the wild on party", 1.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new PartyMemberDeadTrigger(ai),
		NextAction::array(0, new NextAction("revive", 2.0f), NULL)));
}


ActionNode* GenericDruidNonCombatStrategy::createAction(const char* name)
{
    if (!strcmp("mark of the wild", name)) 
    {
        return new ActionNode (new CastMarkOfTheWildAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("mark of the wild on party", name)) 
    {
        return new ActionNode (new CastMarkOfTheWildOnPartyAction(ai),  
            /*P*/ NextAction::array(0, new NextAction("caster form"), NULL),
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
	else if (!strcmp("revive", name)) 
	{
		return new ActionNode (new CastReviveAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else return GenericNonCombatStrategy::createAction(name);
}