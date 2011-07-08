#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "AreaTriggerAction.h"


using namespace ai;

bool ReachAreaTriggerAction::Execute(Event event)
{
    uint32 triggerId;
    WorldPacket p(event.getPacket());
    p.rpos(0);
    p >> triggerId;

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(triggerId);
    if(!atEntry)
        return false;

    AreaTrigger const* at = sObjectMgr.GetAreaTrigger(triggerId);
    if (!at)
    {
        WorldPacket p1(CMSG_AREATRIGGER);
        p1 << triggerId;
        p1.rpos(0);
        bot->GetSession()->HandleAreaTriggerOpcode(p1);

        return true;
    }

    if (bot->GetMapId() != atEntry->mapid || bot->GetDistance(atEntry->x, atEntry->y, atEntry->z) > BOT_SIGHT_DISTANCE)
    {
        ai->TellMaster("I will not follow you - too far away");
        return true;
    }

    ai->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);
    MotionMaster &mm = *bot->GetMotionMaster();
    mm.Clear();
    bot->TeleportTo(atEntry->mapid, atEntry->x, atEntry->y, atEntry->z, 0.0f, TELE_TO_NOT_LEAVE_TRANSPORT);
    bot->SendHeartBeat();
    ai->SetNextCheckDelay(3);
    ai->TellMaster("I will teleport in 3 seconds");

    context->GetValue<LastMovement&>("last movement")->Get().lastAreaTrigger = triggerId;

    return true;
}



bool AreaTriggerAction::Execute(Event event)
{
    LastMovement& movement = context->GetValue<LastMovement&>("last movement")->Get();

    uint32 triggerId = movement.lastAreaTrigger;
    movement.lastAreaTrigger = 0;

    AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(triggerId);
    if(!atEntry)
        return false;

    AreaTrigger const* at = sObjectMgr.GetAreaTrigger(triggerId);
    if (!at)
        return true;

    ai->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);
    MotionMaster &mm = *bot->GetMotionMaster();
    mm.Clear();
    bot->TeleportTo(atEntry->mapid, atEntry->x, atEntry->y, atEntry->z, 0.0f, TELE_TO_NOT_LEAVE_TRANSPORT);
    bot->SendHeartBeat();

    WorldPacket p(CMSG_AREATRIGGER);
    p << triggerId;
    p.rpos(0);
    bot->GetSession()->HandleAreaTriggerOpcode(p);

    return true;
}
