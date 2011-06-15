#pragma once

#include "../GenericActions.h"

namespace ai
{
    class CastGreaterHealAction : public CastHealingSpellAction {
    public:
        CastGreaterHealAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "greater heal") {}
    };

    class CastGreaterHealOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastGreaterHealOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "greater heal") {}

        virtual const char* getName() { return "greater heal on party"; }
    };

    class CastLesserHealAction : public CastHealingSpellAction {
    public:
        CastLesserHealAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "lesser heal") {}
    };

    class CastLesserHealOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastLesserHealOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "lesser heal") {}

        virtual const char* getName() { return "lesser heal on party"; }
    };

    class CastFlashHealAction : public CastHealingSpellAction {
    public:
        CastFlashHealAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "flash heal") {}
    };

    class CastFlashHealOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastFlashHealOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "flash heal") {}

        virtual const char* getName() { return "flash heal on party"; }
    };

    class CastHealAction : public CastHealingSpellAction {
    public:
        CastHealAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "heal") {}
    };

    class CastHealOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastHealOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "heal") {}

        virtual const char* getName() { return "heal on party"; }
    };

    class CastRenewAction : public CastHealingSpellAction {
    public:
        CastRenewAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "renew") {}
    };

    class CastRenewOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastRenewOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "renew") {}

        virtual const char* getName() { return "renew on party"; }
    };

    class CastFadeAction : public CastBuffSpellAction {
    public:
        CastFadeAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "fade") {}
    };

    class CastShadowformAction : public CastBuffSpellAction {
    public:
        CastShadowformAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "shadowform") {}
    };

    class CastRemoveShadowformAction : public Action {
    public:
        CastRemoveShadowformAction(AiManagerRegistry* const ai) : Action(ai, "remove shadowform") {}
        virtual bool isUseful() { return ai->GetAi()->HasAura("shadowform", AI_VALUE(Unit*, "self target")); }
        virtual bool isPossible() { return true; }
        virtual bool Execute(Event event) {
            ai->GetAi()->RemoveAura("shadowform");
            return true;
        }
    };

	class CastPowerWordShieldAction : public CastBuffSpellAction {
	public:
		CastPowerWordShieldAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "power word: shield") {}
	};

    class CastPowerWordShieldOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastPowerWordShieldOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "power word: shield") {}

        virtual const char* getName() { return "power word: shield on party"; }
    };

	class CastPowerWordFortitudeAction : public CastBuffSpellAction {
	public:
		CastPowerWordFortitudeAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "power word: fortitude") {}
	};

	class CastDivineSpiritAction : public CastBuffSpellAction {
	public:
		CastDivineSpiritAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "divine spirit") {}
	};

	class CastInnerFireAction : public CastBuffSpellAction {
	public:
		CastInnerFireAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "inner fire") {}
	};

    BEGIN_SPELL_ACTION(CastHolyNovaAction, "holy nova")
    virtual bool isUseful() {
        return !ai->GetAi()->HasAura("shadowform", AI_VALUE(Unit*, "self target"));
    }
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastHolyFireAction, "holy fire")
        virtual bool isUseful() {
            return !ai->GetAi()->HasAura("shadowform", AI_VALUE(Unit*, "self target"));
        }
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastSmiteAction, "smite")
        virtual bool isUseful() {
			return !ai->GetAi()->HasAura("shadowform", AI_VALUE(Unit*, "self target"));
        }
    END_SPELL_ACTION()

	class CastPowerWordFortitudeOnPartyAction : public BuffOnPartyAction {
	public:
		CastPowerWordFortitudeOnPartyAction(AiManagerRegistry* const ai) : BuffOnPartyAction(ai, "power word: fortitude") {}
	};

	class CastDivineSpiritOnPartyAction : public BuffOnPartyAction {
	public:
		CastDivineSpiritOnPartyAction(AiManagerRegistry* const ai) : BuffOnPartyAction(ai, "divine spirit") {}
	};


    BEGIN_DEBUFF_ACTION(CastPowerWordPainAction, "shadow word: pain")
    END_SPELL_ACTION()

    BEGIN_DEBUFF_ACTION(CastDevouringPlagueAction, "devouring plague")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastMindBlastAction, "mind blast")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastMindFlayAction, "mind flay")
    END_SPELL_ACTION()

	class CastCureDiseaseAction : public CastCureSpellAction {
	public:
		CastCureDiseaseAction(AiManagerRegistry* const ai) : CastCureSpellAction(ai, "cure disease") {}
	};

    class CastCureDiseaseOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastCureDiseaseOnPartyAction(AiManagerRegistry* const ai) : CurePartyMemberAction(ai, "cure disease", DISPEL_DISEASE) {}
        virtual const char* getName() { return "cure disease on party"; }
    };

	class CastAbolishDiseaseAction : public CastCureSpellAction {
	public:
		CastAbolishDiseaseAction(AiManagerRegistry* const ai) : CastCureSpellAction(ai, "abolish disease") {}
		virtual NextAction** getAlternatives();
	};

    class CastAbolishDiseaseOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastAbolishDiseaseOnPartyAction(AiManagerRegistry* const ai) : CurePartyMemberAction(ai, "abolish disease", DISPEL_DISEASE) {}
        virtual const char* getName() { return "abolish disease on party"; }
        virtual NextAction** getAlternatives();
    };

	class CastDispelMagicAction : public CastCureSpellAction {
	public:
		CastDispelMagicAction(AiManagerRegistry* const ai) : CastCureSpellAction(ai, "dispel magic") {}
	};

    class CastDispelMagicOnTargetAction : public CastSpellAction {
    public:
        CastDispelMagicOnTargetAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "dispel magic") {}
    };

    class CastDispelMagicOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastDispelMagicOnPartyAction(AiManagerRegistry* const ai) : CurePartyMemberAction(ai, "dispel magic", DISPEL_MAGIC) {}
        virtual const char* getName() { return "dispel magic on party"; }
    };

	class CastResurrectionAction : public ResurrectPartyMemberAction
	{
	public:
		CastResurrectionAction(AiManagerRegistry* const ai) : ResurrectPartyMemberAction(ai, "resurrection") {}
	};

}