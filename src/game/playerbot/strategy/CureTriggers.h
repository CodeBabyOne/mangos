#pragma once
#include "Trigger.h"

namespace ai
{
	class SpellTrigger;

    class NeedCureTrigger : public SpellTrigger {
    public:
        NeedCureTrigger(AiManagerRegistry* const ai, const char* spell, uint32 dispelType) : SpellTrigger(ai, spell) 
  	    {
			this->dispelType = dispelType;
        }
        virtual const char* GetTargetName() { return "self target"; }
        virtual bool IsActive();

    protected:
        uint32 dispelType;
    };

    class TargetAuraDispelTrigger : public NeedCureTrigger {
    public:
        TargetAuraDispelTrigger(AiManagerRegistry* const ai, const char* spell, uint32 dispelType) : 
			NeedCureTrigger(ai, spell, dispelType) {}
		virtual const char* GetTargetName() { return "current target"; }
    };

    class PartyMemberNeedCureTrigger : public NeedCureTrigger {
    public:
        PartyMemberNeedCureTrigger(AiManagerRegistry* const ai, const char* spell, uint32 dispelType) : 
            NeedCureTrigger(ai, spell, dispelType) {}

		virtual Value<Unit*>* GetTargetValue();
    };
}