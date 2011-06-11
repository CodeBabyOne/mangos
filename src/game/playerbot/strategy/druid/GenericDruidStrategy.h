#pragma once

#include "../Strategy.h"

namespace ai
{
    class AiObjectContext;

    class GenericDruidStrategy : public CombatStrategy
    {
    protected:
        GenericDruidStrategy(AiManagerRegistry* const ai) : CombatStrategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual AiObjectContext* createAiObjectContext();
        virtual ActionNode* GetAction(const char* name);
    };
}