#pragma once

#include "GenericDruidStrategy.h"

namespace ai
{
    class CasterDruidStrategy : public GenericDruidStrategy
    {
    public:
        CasterDruidStrategy(AiManagerRegistry* const ai) : GenericDruidStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "caster"; }
        virtual ActionNode* GetAction(const char* name);
        virtual NextAction** getDefaultActions();
		virtual string GetIncompatibleStrategies() { return "-cat,-bear"; }
    };
}