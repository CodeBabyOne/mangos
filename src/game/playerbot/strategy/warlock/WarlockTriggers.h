#pragma once
#include "../GenericTriggers.h"

namespace ai
{
	class DemonArmorTrigger : public BuffTrigger
	{
	public:
		DemonArmorTrigger(AiManagerRegistry* const ai) : BuffTrigger(ai, "demon armor") {}
		virtual bool IsActive();
	};

    class SpellstoneTrigger : public BuffTrigger
    {
    public:
        SpellstoneTrigger(AiManagerRegistry* const ai) : BuffTrigger(ai, "spellstone") {}
        virtual bool IsActive();
    };

    DEBUFF_TRIGGER(CurseOfAgonyTrigger, "curse of agony", "curse of agony");
    DEBUFF_TRIGGER(CorruptionTrigger, "corruption", "corruption");
    DEBUFF_TRIGGER(ImmolateTrigger, "immolate", "immolate");

    class ShadowTranceTrigger : public HasAuraTrigger
    {
    public:
        ShadowTranceTrigger(AiManagerRegistry* const ai) : HasAuraTrigger(ai, "shadow trance") {}
    };
    
}