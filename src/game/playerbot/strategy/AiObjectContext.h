#pragma once

#include "../PlayerbotAIAware.h"
#include "Action.h"
#include "Strategy.h"
#include "Value.h"
#include "NamedObjectContext.h"

namespace ai
{
    class AiObjectContext : public PlayerbotAIAware
    {
    public:
        AiObjectContext(PlayerbotAI* ai);
        virtual ~AiObjectContext() {}

    public:
        virtual Strategy* GetStrategy(string name) { return strategyContexts.GetObject(name, ai); }
        set<string> GetSiblingStrategy(string name) { return strategyContexts.GetSiblings(name); }
        virtual Trigger* GetTrigger(string name) { return triggerContexts.GetObject(name, ai); }
        virtual Action* GetAction(string name) { return actionContexts.GetObject(name, ai); }
        virtual UntypedValue* GetUntypedValue(string name) { return valueContexts.GetObject(name, ai); }

        template<class T>
        Value<T>* GetValue(string name)
        {
            return dynamic_cast<Value<T>*>(GetUntypedValue(name));
        }

        template<class T>
        Value<T>* GetValue(string name, string param)
        {
            return GetValue<T>((string(name) + "::" + param));
        }

        template<class T>
        Value<T>* GetValue(string name, uint32 param)
        {
            char buffer[64]; itoa(param, buffer, 10);
            return GetValue<T>(name, buffer);
        }

    public:
        virtual void Update();
        virtual void AddShared(NamedObjectContext<UntypedValue>* sharedValues)
        {
            valueContexts.Add(sharedValues);
        }

    protected:
        NamedObjectContextList<Strategy> strategyContexts;
        NamedObjectContextList<Action> actionContexts;
        NamedObjectContextList<Trigger> triggerContexts;
        NamedObjectContextList<UntypedValue> valueContexts;
    };
}
