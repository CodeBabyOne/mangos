#pragma once

#include "../GenericActions.h"
#include "DruidShapeshiftActions.h"
#include "DruidBearActions.h"
#include "DruidCatActions.h"

namespace ai
{
	class CastFaerieFireAction : public CastSpellAction 
	{ 
	public: 
		CastFaerieFireAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "faerie fire") {} 
	};

    class CastFaerieFireFeralAction : public CastSpellAction 
    { 
    public: 
        CastFaerieFireFeralAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "faerie fire (feral)") {} 
    };
  
	class CastRejuvenationAction : public CastHealingSpellAction {
	public:
		CastRejuvenationAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "rejuvenation") {}

		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), CastHealingSpellAction::getPrerequisites());
		}

	};

	class CastRegrowthAction : public CastHealingSpellAction {
	public:
		CastRegrowthAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "regrowth") {}

		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), CastHealingSpellAction::getPrerequisites());
		}

	};
   
    class CastHealingTouchAction : public CastHealingSpellAction {
    public:
        CastHealingTouchAction(AiManagerRegistry* const ai) : CastHealingSpellAction(ai, "healing touch") {}

        virtual NextAction** getPrerequisites() {
            return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), CastHealingSpellAction::getPrerequisites());
        }

    };

    class CastRejuvenationOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastRejuvenationOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "rejuvenation") {}
		
		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), HealPartyMemberAction::getPrerequisites());
		}
    };

    class CastRegrowthOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastRegrowthOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "regrowth") {}
		
		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), HealPartyMemberAction::getPrerequisites());
		}
    };

    class CastHealingTouchOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastHealingTouchOnPartyAction(AiManagerRegistry* const ai) : HealPartyMemberAction(ai, "healing touch") {}

        virtual NextAction** getPrerequisites() {
            return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), HealPartyMemberAction::getPrerequisites());
        }
    };

	class CastReviveAction : public ResurrectPartyMemberAction
	{
	public:
		CastReviveAction(AiManagerRegistry* const ai) : ResurrectPartyMemberAction(ai, "revive") {}

		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), ResurrectPartyMemberAction::getPrerequisites());
		}
	};

	class CastRebirthAction : public ResurrectPartyMemberAction
	{
	public:
		CastRebirthAction(AiManagerRegistry* const ai) : ResurrectPartyMemberAction(ai, "rebirth") {}

		virtual NextAction** getPrerequisites() {
			return NextAction::merge( NextAction::array(0, new NextAction("caster form"), NULL), ResurrectPartyMemberAction::getPrerequisites());
		}
	};

	class CastMarkOfTheWildAction : public CastBuffSpellAction {
	public:
		CastMarkOfTheWildAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "mark of the wild") {}
	};

	class CastMarkOfTheWildOnPartyAction : public BuffOnPartyAction {
	public:
		CastMarkOfTheWildOnPartyAction(AiManagerRegistry* const ai) : BuffOnPartyAction(ai, "mark of the wild") {}
	};

	class CastSurvivalInstinctsAction : public CastBuffSpellAction {
	public:
		CastSurvivalInstinctsAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "survival instincts") {}
	};

	class CastThornsAction : public CastBuffSpellAction {
	public:
		CastThornsAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "thorns") {}
	};

	class CastWrathAction : public CastSpellAction 
	{ 
	public: 
		CastWrathAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "wrath") {} 
	};

	class CastStarfallAction : public CastSpellAction 
	{ 
	public: 
		CastStarfallAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "starfall") {} 
	};

	class CastMoonfireAction : public CastDebuffSpellAction 
	{ 
	public: 
		CastMoonfireAction(AiManagerRegistry* const ai) : CastDebuffSpellAction(ai, "moonfire") {} 
	};

	class CastInsectSwarmAction : public CastDebuffSpellAction 
	{ 
	public: 
		CastInsectSwarmAction(AiManagerRegistry* const ai) : CastDebuffSpellAction(ai, "insect swarm") {} 
	};

	class CastStarfireAction : public CastSpellAction 
	{ 
	public: 
		CastStarfireAction(AiManagerRegistry* const ai) : CastSpellAction(ai, "starfire") {} 
	};

	class CastEntanglingRootsAction : public CastDebuffSpellAction 
	{ 
	public: 
		CastEntanglingRootsAction(AiManagerRegistry* const ai) : CastDebuffSpellAction(ai, "entangling roots") {} 
	};

    class CastEntanglingRootsCcAction : public CastDebuffSpellAction 
    { 
    public: 
        CastEntanglingRootsCcAction(AiManagerRegistry* const ai) : CastDebuffSpellAction(ai, "entangling roots on cc") {} 
        virtual Value<Unit*>* GetTargetValue();
        virtual bool Execute(Event event);
    };

	class CastNaturesGraspAction : public CastBuffSpellAction 
	{ 
	public: 
		CastNaturesGraspAction(AiManagerRegistry* const ai) : CastBuffSpellAction(ai, "nature's grasp") {} 
	};
	
	class CastHibernateAction : public CastDebuffSpellAction 
	{ 
	public: 
		CastHibernateAction(AiManagerRegistry* const ai) : CastDebuffSpellAction(ai, "hibernate") {} 
	};

	class CastCurePoisonAction : public CastCureSpellAction 
	{ 
	public: 
		CastCurePoisonAction(AiManagerRegistry* const ai) : CastCureSpellAction(ai, "cure poison") {} 
	};

    class CastCurePoisonOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastCurePoisonOnPartyAction(AiManagerRegistry* const ai) : CurePartyMemberAction(ai, "cure poison", DISPEL_POISON) {}
    };

	class CastAbolishPoisonAction : public CastCureSpellAction 
	{ 
	public: 
		CastAbolishPoisonAction(AiManagerRegistry* const ai) : CastCureSpellAction(ai, "abolish poison") {} 
		virtual NextAction** getAlternatives();
	};

    class CastAbolishPoisonOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastAbolishPoisonOnPartyAction(AiManagerRegistry* const ai) : CurePartyMemberAction(ai, "abolish poison", DISPEL_POISON) {}

        virtual NextAction** getAlternatives();
    };

}