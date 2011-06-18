#pragma once

#include "../Action.h"
#include "MovementActions.h"

namespace ai
{
	class AttackAction : public MovementAction {
	public:
		AttackAction(PlayerbotAI* ai, const char* name) : MovementAction(ai, name) {}

    public:
        virtual bool Execute(Event event);
        virtual bool isPossible() { return GetTarget(); }

    protected:
        bool Attack(Unit* target);
    };

    class AttackMyTargetAction : public AttackAction {
    public:
        AttackMyTargetAction(PlayerbotAI* ai, const char* name = "attack my target") : AttackAction(ai, name) {}

    public:
        virtual bool Execute(Event event);
    };
}