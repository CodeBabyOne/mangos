#pragma once

#include "Strategy.h"
#include "ActionFactory.h"

namespace ai
{
    class GenericNonCombatStrategy : public Strategy
    {
    public:
        GenericNonCombatStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}
		virtual StrategyType GetType() { return STRATEGY_TYPE_NONCOMBAT; }
    };

    class FollowMasterNonCombatStrategy : public GenericNonCombatStrategy
    {
    public:
        FollowMasterNonCombatStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "follow master"; }
        virtual ActionNode* createAction(const char* name);
        virtual NextAction** getDefaultActions();
		virtual void InitTriggers(std::list<TriggerNode*> &triggers);
		virtual string GetIncompatibleStrategies() { return "-stay,-go away,-follow line"; }

    };

	class FollowLineNonCombatStrategy : public GenericNonCombatStrategy
	{
	public:
		FollowLineNonCombatStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
		virtual const char* getName() { return "follow line"; }
		virtual ActionNode* createAction(const char* name);
		virtual NextAction** getDefaultActions();
		virtual string GetIncompatibleStrategies() { return "-stay,-go away,-follow master"; }
	};

    class GoAwayNonCombatStrategy : public GenericNonCombatStrategy
    {
    public:
        GoAwayNonCombatStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "goaway"; }
        virtual ActionNode* createAction(const char* name);
        virtual NextAction** getDefaultActions();
		virtual string GetIncompatibleStrategies() { return "-stay,-follow line,-follow master"; }
    };

    class StayNonCombatStrategy : public GenericNonCombatStrategy
    {
    public:
        StayNonCombatStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "stay"; }
        virtual ActionNode* createAction(const char* name);
        virtual NextAction** getDefaultActions();
		virtual string GetIncompatibleStrategies() { return "-go away,-follow line,-follow master"; }
    };

    class DpsAssistStrategy : public GenericNonCombatStrategy
    {
    public:
        DpsAssistStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "dps assist"; }
		virtual StrategyType GetType() { return STRATEGY_TYPE_DPS; }
		virtual string GetIncompatibleStrategies() { return "-tank assist,-dps aoe,-tank aoe,-grind"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

	class DpsAoeStrategy : public GenericNonCombatStrategy
	{
	public:
		DpsAoeStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
		virtual const char* getName() { return "dps aoe"; }
		virtual StrategyType GetType() { return STRATEGY_TYPE_DPS; }
		virtual string GetIncompatibleStrategies() { return "-tank assist,-dps assist,-tank aoe,-grind"; }

	public:
		virtual void InitTriggers(std::list<TriggerNode*> &triggers);
	};

    class TankAssistStrategy : public GenericNonCombatStrategy
    {
    public:
        TankAssistStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "tank assist"; }
		virtual StrategyType GetType() { return STRATEGY_TYPE_TANK; }
		virtual string GetIncompatibleStrategies() { return "-dps assist,-dps aoe,-tank aoe,-grind"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

	class TankAoeStrategy : public GenericNonCombatStrategy
	{
	public:
		TankAoeStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
		virtual const char* getName() { return "tank aoe"; }
		virtual StrategyType GetType() { return STRATEGY_TYPE_TANK; }
		virtual string GetIncompatibleStrategies() { return "-tank assist,-dps assist,-dps aoe,-grind"; }

	public:
		virtual void InitTriggers(std::list<TriggerNode*> &triggers);
	};

    class GrindingStrategy : public GenericNonCombatStrategy
    {
    public:
        GrindingStrategy(AiManagerRegistry* const ai) : GenericNonCombatStrategy(ai) {}
        virtual const char* getName() { return "grind"; }
		virtual StrategyType GetType() { return STRATEGY_TYPE_DPS; }
		virtual string GetIncompatibleStrategies() { return "-tank assist,-dps assist,-dps aoe,-tank aoe"; }
		NextAction** getDefaultActions();

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
		virtual ActionNode* createAction(const char* name);
    };

    class LootNonCombatStrategy : public Strategy
    {
    public:
        LootNonCombatStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual const char* getName() { return "loot"; }
    };

    class RandomEmoteStrategy : public Strategy
    {
    public:
        RandomEmoteStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual const char* getName() { return "emote"; }
        virtual ActionNode* createAction(const char* name);
   };

    class PassiveStrategy : public Strategy
    {
    public:
        PassiveStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}

    public:
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "passive"; }
    };

    class LowManaStrategy : public Strategy
    {
    public:
        LowManaStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}

    public:
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual const char* getName() { return "low mana"; }
    };

    class UseFoodStrategy : public Strategy
    {
    public:
        UseFoodStrategy(AiManagerRegistry* const ai) : Strategy(ai) {}

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual const char* getName() { return "food"; }
    };
}