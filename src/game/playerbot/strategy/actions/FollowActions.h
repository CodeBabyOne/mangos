#pragma once

#include "../Action.h"
#include "MovementActions.h"

namespace ai
{
	class FollowAction : public MovementAction {
	public:
		FollowAction(PlayerbotAI* ai, const char* name) : MovementAction(ai, name) {}
	};

	class FollowLineAction : public FollowAction {
	public:
		FollowLineAction(PlayerbotAI* ai) : FollowAction(ai, "follow line") {}
		virtual bool Execute(Event event);
	};

	class FollowMasterAction : public MovementAction {
	public:
		FollowMasterAction(PlayerbotAI* ai) : MovementAction(ai, "follow master") {}
		virtual bool Execute(Event event);
	};

    class FollowMasterRandomAction : public MovementAction {
    public:
        FollowMasterRandomAction(PlayerbotAI* ai) : MovementAction(ai, "be near") {}
        virtual bool Execute(Event event);
    };
}