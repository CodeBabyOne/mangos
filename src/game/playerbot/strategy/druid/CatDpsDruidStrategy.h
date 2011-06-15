#pragma once

#include "FeralDruidStrategy.h"

namespace ai
{
    class CatDpsDruidStrategy : public FeralDruidStrategy
    {
    public:
        CatDpsDruidStrategy(PlayerbotAI* ai) : FeralDruidStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "cat"; }
        virtual ActionNode* GetAction(const char* name);
        virtual NextAction** getDefaultActions();
		virtual string GetIncompatibleStrategies() { return "-bear,-caster"; }
    };
}