#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "ShamanTriggers.h"
#include "ShamanMultipliers.h"
#include "ShamanNonCombatStrategy.h"
#include "ShamanActions.h"

using namespace ai;

void ShamanNonCombatStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

	triggers.push_back(new TriggerNode(
		new PartyMemberDeadTrigger(ai),
		NextAction::array(0, new NextAction("ancestral spirit", 23.0f), NULL)));
}

void ShamanNonCombatStrategy::InitMultipliers(std::list<Multiplier*> &multipliers)
{
    CombatStrategy::InitMultipliers(multipliers);
}

ActionNode* ShamanNonCombatStrategy::createAction(const char* name)
{
    ActionNode* node = CombatStrategy::createAction(name);
    if (node)
        return node;

    if (!strcmp("strength of earth totem", name)) 
    {
        return new ActionNode (new CastStrengthOfEarthTotemAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("flametongue totem", name)) 
    {
        return new ActionNode (new CastFlametongueTotemAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("windfury totem", name)) 
    {
        return new ActionNode (new CastWindfuryTotemAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
    else if (!strcmp("mana spring totem", name)) 
    {
        return new ActionNode (new CastManaSpringTotemAction(ai),  
            /*P*/ NULL,
            /*A*/ NULL, 
            /*C*/ NULL);
    }
	else if (!strcmp("ancestral spirit", name)) 
	{
		return new ActionNode (new CastAncestralSpiritAction(ai),  
			/*P*/ NULL,
			/*A*/ NULL, 
			/*C*/ NULL);
	}
    else return NULL;
}



