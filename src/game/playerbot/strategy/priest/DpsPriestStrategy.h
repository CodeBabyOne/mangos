#pragma once

#include "HealPriestStrategy.h"

namespace ai
{
    class DpsPriestStrategy : public HealPriestStrategy
    {
    public:
        DpsPriestStrategy(PlayerbotAI* ai) : HealPriestStrategy(ai) {}

    public:
        virtual NextAction** getDefaultActions();
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "dps"; }
        virtual ActionNode* GetAction(const char* name);

    };
}