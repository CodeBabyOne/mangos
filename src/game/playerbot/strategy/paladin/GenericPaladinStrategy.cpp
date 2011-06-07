#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "PaladinTriggers.h"
#include "GenericPaladinStrategy.h"
#include "PaladinActions.h"

using namespace ai;

void GenericPaladinStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
	CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        new EnemyOutOfMeleeTrigger(ai),
        NextAction::array(0, new NextAction("melee", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new HammerOfJusticeSnareTrigger(ai),
        NextAction::array(0, new NextAction("hammer of justice", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new MediumHealthTrigger(ai),
        NextAction::array(0, new NextAction("flash of light", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new PartyMemberMediumHealthTrigger(ai),
        NextAction::array(0, new NextAction("flash of light on party", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new LowHealthTrigger(ai),
        NextAction::array(0, new NextAction("divine shield", 50.0f), new NextAction("holy light", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new PartyMemberLowHealthTrigger(ai),
        NextAction::array(0, new NextAction("divine protection on party", 40.0f), new NextAction("holy light on party", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new HammerOfJusticeInterruptSpellTrigger(ai),
		NextAction::array(0, new NextAction("hammer of justice", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new CriticalHealthTrigger(ai),
		NextAction::array(0, new NextAction("lay on hands", 90.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new PartyMemberCriticalHealthTrigger(ai),
		NextAction::array(0, new NextAction("lay on hands on party", 90.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new TargetCriticalHealthTrigger(ai),
		NextAction::array(0, new NextAction("hammer of wrath", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CleanseCureDiseaseTrigger(ai),
        NextAction::array(0, new NextAction("cleanse", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CleanseCurePartyMemberDiseaseTrigger(ai),
        NextAction::array(0, new NextAction("cleanse disease on party", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CleanseCurePoisonTrigger(ai),
        NextAction::array(0, new NextAction("cleanse", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        new CleanseCurePartyMemberPoisonTrigger(ai),
        NextAction::array(0, new NextAction("cleanse poison on party", 40.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new CleanseCureMagicTrigger(ai),
		NextAction::array(0, new NextAction("cleanse", 41.0f), NULL)));

	triggers.push_back(new TriggerNode(
		new CleanseCurePartyMemberMagicTrigger(ai),
		NextAction::array(0, new NextAction("cleanse magic on party", 40.0f), NULL)));
}


ActionNode* GenericPaladinStrategy::createAction(const char* name)
{
    if (!strcmp("seal of light", name))
    {
        return new ActionNode (new CastSealOfLightAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("seal of justice"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("devotion aura", name))
    {
        return new ActionNode (new CastDevotionAuraAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("holy wrath", name))
    {
        return new ActionNode (new CastHolyWrathAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("consecration", name))
    {
        return new ActionNode (new CastConsecrationAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("cleanse", name))
    {
        return new ActionNode (new CastCleanseAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("purify"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("purify", name))
    {
        return new ActionNode (new CastPurifyAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("cleanse poison on party", name))
    {
        return new ActionNode (new CastCleansePoisonOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("purify poison on party"), NULL),
            /*C*/ NULL);
    }
	else if (!strcmp("cleanse disease on party", name))
	{
		return new ActionNode (new CastCleanseDiseaseOnPartyAction(ai),
			/*P*/ NULL,
			/*A*/ NextAction::array(0, new NextAction("purify disease on party"), NULL),
			/*C*/ NULL);
	}
	else if (!strcmp("cleanse magic on party", name))
	{
		return new ActionNode (new CastCleanseMagicOnPartyAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("purify poison on party", name))
    {
        return new ActionNode (new CastPurifyPoisonOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("purify disease on party", name))
	{
		return new ActionNode (new CastPurifyDiseaseOnPartyAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("seal of wisdom", name))
    {
        return new ActionNode (new CastSealOfWisdomAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("seal of justice"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("seal of justice", name))
    {
        return new ActionNode (new CastSealOfJusticeAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("seal of righteousness"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("seal of righteousness", name))
    {
        return new ActionNode (new CastSealOfRighteousnessAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("flash of light", name))
    {
        return new ActionNode (new CastFlashOfLightAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("hand of reckoning", name))
    {
        return new ActionNode (new CastHandOfReckoningAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("judgement of justice"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("avenger's shield", name))
    {
        return new ActionNode (new CastAvengersShieldAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("exorcism", name))
    {
        return new ActionNode (new CastExorcismAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("judgement of light", name))
    {
        return new ActionNode (new CastJudgementOfLightAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("judgement of wisdom", name))
    {
        return new ActionNode (new CastJudgementOfWisdomAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("judgement of light"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("divine shield", name))
    {
        return new ActionNode (new CastDivineShieldAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("divine protection"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("divine protection", name))
    {
        return new ActionNode (new CastDivineProtectionAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("divine protection on party", name))
    {
        return new ActionNode (new CastDivineProtectionOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("hammer of justice", name))
    {
        return new ActionNode (new CastHammerOfJusticeAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("flash of light", name))
    {
        return new ActionNode (new CastFlashOfLightAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("holy light"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("flash of light on party", name))
    {
        return new ActionNode (new CastFlashOfLightOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("holy light on party"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("holy light", name))
    {
        return new ActionNode (new CastHolyLightAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("holy light on party", name))
    {
        return new ActionNode (new CastHolyLightOnPartyAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("lay on hands", name))
	{
		return new ActionNode (new CastLayOnHandsAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("lay on hands on party", name))
	{
		return new ActionNode (new CastLayOnHandsOnPartyAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("judgement of justice", name))
	{
		return new ActionNode (new CastJudgementOfJusticeAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("hammer of wrath", name))
	{
		return new ActionNode (new CastHammerOfWrathAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
    else if (!strcmp("holy shield", name))
    {
        return new ActionNode (new CastHolyShieldAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("hammer of the righteous", name))
    {
        return new ActionNode (new CastHammerOfTheRighteousAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("holy wrath", name))
    {
        return new ActionNode (new CastHolyWrathAction(ai),
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("consecration"), NULL),
            /*C*/ NULL);
    }
    else if (!strcmp("consecration", name))
    {
        return new ActionNode (new CastConsecrationAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
    else if (!strcmp("blessing of kings", name))
    {
        return new ActionNode (new CastBlessingOfKingsAction(ai),
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
	else if (!strcmp("retribution aura", name))
	{
		return new ActionNode (new CastRetributionAuraAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("shadow resistance aura", name))
	{
		return new ActionNode (new CastShadowResistanceAuraAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("frost resistance aura", name))
	{
		return new ActionNode (new CastFrostResistanceAuraAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else if (!strcmp("fire resistance aura", name))
	{
		return new ActionNode (new CastFireResistanceAuraAction(ai),
			/*P*/ NULL,
			/*A*/ NULL,
			/*C*/ NULL);
	}
	else return NULL;
}
