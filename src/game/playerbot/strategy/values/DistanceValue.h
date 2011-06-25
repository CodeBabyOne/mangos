#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "../../LootObjectStack.h"

namespace ai
{
    class DistanceValue : public CalculatedValue<float>, public Qualified
	{
	public:
        DistanceValue(PlayerbotAI* ai) : CalculatedValue<float>(ai) {}

    public:
        float Calculate()
        {
            if (qualifier == "loot target")
            {
                LootObject loot = AI_VALUE(LootObject, qualifier);
                if (loot.IsEmpty())
                    return 0.0f;
                
                return ai->GetBot()->GetDistance(loot.GetWorldObject(bot));
            }
            Unit* target = AI_VALUE(Unit*, qualifier);
            if (!target)
                return 0.0f;

            return ai->GetBot()->GetDistance(target);
        }
    };
}
