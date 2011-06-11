#pragma once

#include "../Strategy.h"

namespace ai
{
    class HealPriestNonCombatStrategy : public CombatStrategy
    {
    public:
        HealPriestNonCombatStrategy(AiManagerRegistry* const ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "nc"; }
        virtual ActionNode* GetAction(const char* name);

    };
}