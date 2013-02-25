#pragma once

#include "../Action.h"
#include "AttackAction.h"

namespace ai
{
    class DpsAssistAction : public AttackAction {
    public:
        DpsAssistAction(PlayerbotAI* ai) : AttackAction(ai, "dps assist") {}

        virtual string GetTargetName() { return "dps target"; }
    };

    class TankAssistAction : public AttackAction {
    public:
        TankAssistAction(PlayerbotAI* ai) : AttackAction(ai, "tank assist") {}
        virtual string GetTargetName() { return "tank target"; }
    };

    class AttackAnythingAction : public AttackAction {
    public:
        AttackAnythingAction(PlayerbotAI* ai) : AttackAction(ai, "attack anything") {}
        virtual string GetTargetName() { return "grind target"; }
    };

    class AttackLeastHpTargetAction : public AttackAction {
    public:
        AttackLeastHpTargetAction(PlayerbotAI* ai) : AttackAction(ai, "attack least hp target") {}
        virtual string GetTargetName() { return "least hp target"; }
    };

    class AttackRtiTargetAction : public AttackAction {
    public:
        AttackRtiTargetAction(PlayerbotAI* ai) : AttackAction(ai, "attack rti target") {}
        virtual string GetTargetName() { return "rti target"; }
    };

    class DropTargetAction : public Action {
    public:
        DropTargetAction(PlayerbotAI* ai) : Action(ai, "drop target") {}

        virtual bool Execute(Event event)
        {
            context->GetValue<Unit*>("current target")->Set(NULL);
            bot->SetSelectionGuid(ObjectGuid());
            ai->ChangeEngine(BOT_STATE_NON_COMBAT);
			return true;
        }
    };

}
