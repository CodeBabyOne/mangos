#pragma once

#include "../Strategy.h"

namespace ai
{
    class GenericMageStrategy : public CombatStrategy
    {
    public:
        GenericMageStrategy(AiManagerRegistry* const ai) : CombatStrategy(ai) {}
        virtual const char* getName() { return "mage"; }
    
    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual ActionNode* GetAction(const char* name);
    };
}