#pragma once

namespace ai
{
    class ConserveManaMultiplier : public Multiplier
    {
    public:
        ConserveManaMultiplier(PlayerbotAI* ai) : Multiplier(ai) {}

    public:
        virtual float GetValue(Action* action);
    };

    class ConserveManaStrategy : public Strategy
    {
    public:
        ConserveManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}

    public:
        virtual void InitMultipliers(std::list<Multiplier*> &multipliers);
        virtual string getName() { return "conserve mana"; }
    };


}
