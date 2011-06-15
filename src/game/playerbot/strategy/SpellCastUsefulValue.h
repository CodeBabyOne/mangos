#pragma once
#include "Value.h"
#include "TargetValue.h"
#include "../AttackerMapProvider.h"

namespace ai
{
   
    class SpellCastUsefulValue : public CalculatedValue<bool>, public Qualified
	{
	public:
        SpellCastUsefulValue(PlayerbotAI* ai) : CalculatedValue<bool>(ai) {}
      
    public:
        virtual bool Calculate();

    };
}
