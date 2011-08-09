#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "../../LootObjectStack.h"

namespace ai
{
    class HasTotemValue : public CalculatedValue<bool>, public Qualified
	{
	public:
        HasTotemValue(PlayerbotAI* ai) : CalculatedValue<bool>(ai) {}

    public:
        bool Calculate()
        {
            list<ObjectGuid> units = *context->GetValue<list<ObjectGuid> >("nearest npcs");
            for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); i++)
            {
                Unit* unit = ai->GetUnit(*i);
                if (!unit)
                    continue;

                Creature* creature = dynamic_cast<Creature*>(unit);
                if (!creature || !creature->IsTotem())
                    continue;

                if (!strstri(qualifier.c_str(), creature->GetName()))
                    return true;
            }

            return false;
        }
    };
}
