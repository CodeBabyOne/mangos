#pragma once
#include "../GenericTriggers.h"

namespace ai
{
	BUFF_TRIGGER(HolyShieldTrigger, "holy shield", "holy shield")
    BUFF_TRIGGER(RighteousFuryTrigger, "righteous fury", "righteous fury")

    BUFF_TRIGGER(DevotionAuraTrigger, "devotion aura", "devotion aura")
    BUFF_TRIGGER(RetributionAuraTrigger, "retribution aura", "retribution aura")

	class CrusaderAuraTrigger : public BuffTrigger
	{
	public:
		CrusaderAuraTrigger(AiManagerRegistry* const ai) : BuffTrigger(ai, "crusader aura") {}
		virtual bool IsActive();
	};

	class SealTrigger : public BuffTrigger
	{
	public:
		SealTrigger(AiManagerRegistry* const ai) : BuffTrigger(ai, "seal of justice") {}
		virtual bool IsActive();
	};

    DEBUFF_TRIGGER(JudgementOfLightTrigger, "judgement of light", "judgement of light")

    BUFF_ON_PARTY_TRIGGER(BlessingOfKingsOnPartyTrigger, "blessing of kings", "blessing of kings on party")
    BUFF_TRIGGER(BlessingTrigger, "blessing of sanctuary", "blessing of sanctuary")

    class HammerOfJusticeInterruptSpellTrigger : public InterruptSpellTrigger
    {
    public:
        HammerOfJusticeInterruptSpellTrigger(AiManagerRegistry* const ai) : InterruptSpellTrigger(ai, "hammer of justice") {}
    };

    class ArtOfWarTrigger : public HasAuraTrigger
    {
    public:
        ArtOfWarTrigger(AiManagerRegistry* const ai) : HasAuraTrigger(ai, "the art of war") {}
    };
}
