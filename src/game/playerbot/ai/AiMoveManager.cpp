#include "../../pchdef.h"
#include "../playerbot.h"
#include "../../MotionMaster.h"
#include "../../MovementGenerator.h"
#include "FleeManager.h"
#include "../../CreatureAI.h"

using namespace ai;
using namespace std;

AiMoveManager::AiMoveManager(PlayerbotAI* ai, AiManagerRegistry* aiRegistry) : AiManagerBase(ai, aiRegistry)
{
	lastAreaTrigger = 0;
}

float AiMoveManager::GetDistanceTo(Unit* target)
{
	return target ? bot->GetDistance(target) : 0.0f; 
}

void AiMoveManager::MoveTo(Unit* target, float distance)
{
    if (!IsMovingAllowed(target))
        return;

	if (distance < SPELL_DISTANCE) 
	{
		Follow(target, distance);
		return;
	}

    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();

    float tx = target->GetPositionX();
    float ty = target->GetPositionY();
    float tz = target->GetPositionZ();

    float distanceToTarget = bot->GetDistance(target);

	float angle = bot->GetAngle(target);

    float destinationDistance = distanceToTarget - distance;
    float dx = cos(angle) * destinationDistance + bx;
    float dy = sin(angle) * destinationDistance + by;

	bot->UpdateGroundPositionZ(dx, dy, tz);
	MoveTo(target->GetMapId(), dx, dy, tz);
}

void AiMoveManager::MoveTo(WorldObject* target)
{
    MoveTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
}

float AiMoveManager::GetFollowAngle()
{
	Group* group = bot->GetGroup();
	if (!group)
		return 0.0f;

	GroupReference *gref = group->GetFirstMember();
	int index = 1;
	while( gref )
	{
		if( gref->getSource() == bot)
		{
			return 2 * M_PI / group->GetMembersCount() * index;
		}
		gref = gref->next();
		index++;
	}
	return 0;
}

bool AiMoveManager::IsMovingAllowed(Unit* target) 
{
    if (!target)
        return false;

    if (bot->GetMapId() != target->GetMapId())
        return false;

    if (bot->GetDistance(target) > BOT_REACT_DISTANCE)
	{
		aiRegistry->GetSocialManager()->TellMaster(LOG_LVL_DEBUG, "I am too far away");
        return false;
	}

    return IsMovingAllowed();
}

bool AiMoveManager::IsMovingAllowed(uint32 mapId, float x, float y, float z)
{
    if (bot->GetMapId() != mapId || bot->GetDistance(x, y, z) > BOT_REACT_DISTANCE)
	{
		aiRegistry->GetSocialManager()->TellMaster(LOG_LVL_DEBUG, "I am too far away");
        return false;
	}

    return IsMovingAllowed();
}

bool AiMoveManager::IsMovingAllowed() 
{
    if (bot->isFrozen() || bot->IsPolymorphed() || !bot->CanFreeMove() || bot->isDead())
        return false;

    MotionMaster &mm = *bot->GetMotionMaster();
    return mm->GetMovementGeneratorType() != FLIGHT_MOTION_TYPE;
}

void AiMoveManager::Follow(Unit* target, float distance)
{
	Follow(target, distance, GetFollowAngle());
}

void AiMoveManager::Follow(Unit* target, float distance, float angle)
{
    if (!IsMovingAllowed(target))
        return;

	if (target->IsFriendlyTo(bot) && bot->IsMounted())
		distance += angle;

	MotionMaster &mm = *bot->GetMotionMaster();
	mm.Clear();
    mm.MoveFollow(target, distance, angle);

    float distanceToRun = abs(bot->GetDistance(target) - distance);
    WaitForReach(distanceToRun);

}

void AiMoveManager::MoveTo(uint32 mapId, float x, float y, float z)
{
    if (!IsMovingAllowed(mapId, x, y, z))
        return;

	MotionMaster &mm = *bot->GetMotionMaster();
	mm.Clear();
    mm.MovePoint(mapId, x, y, z);
    WaitForReach(bot->GetDistance(x, y, z));
}

bool AiMoveManager::Flee(Unit* target, float distance)
{
    if (!IsMovingAllowed(target))
        return true;
    
    AttackerMap attackers = bot->GetPlayerbotAI()->GetGroupStatsManager()->GetAttackers();
	FleeManager manager(bot, &attackers, distance, GetFollowAngle());
        
    float rx, ry, rz;
	if (!manager.CalculateDestination(&rx, &ry, &rz)) 
        return false;

	bot->GetMotionMaster()->MovePoint(0, rx, ry, rz);
    WaitForReach(bot->GetDistance(rx, ry, rz));
	return true;
}

void AiMoveManager::Stay()
{
    MotionMaster &mm = *bot->GetMotionMaster();
    if (mm->GetMovementGeneratorType() == FLIGHT_MOTION_TYPE)
        return;

	mm.Clear();
	bot->clearUnitState( UNIT_STAT_CHASE );
	bot->clearUnitState( UNIT_STAT_FOLLOW );

	if (!bot->IsStandState())
		bot->SetStandState(UNIT_STAND_STATE_STAND);
}

bool AiMoveManager::IsMoving(Unit* target)
{
	return target && target->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE;
}

void AiMoveManager::SetInFront(const Unit* obj)
{
	if (IsMoving(bot))
		return;

	bot->SetInFront(obj);

	float ori = bot->GetAngle(obj);
	float x, y, z;
	x = bot->m_movementInfo.GetPos()->x;
	y = bot->m_movementInfo.GetPos()->y;
	z = bot->m_movementInfo.GetPos()->z;
	bot->m_movementInfo.ChangePosition(x, y, z, ori);

	bot->SendHeartBeat(false);
}

void AiMoveManager::Attack(Unit* target)
{
	if (!target) 
		return;

    MotionMaster &mm = *bot->GetMotionMaster();
    if (mm->GetMovementGeneratorType() == FLIGHT_MOTION_TYPE)
        return;

	if (bot->getStandState() != UNIT_STAND_STATE_STAND)
		bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (aiRegistry->GetStatsManager()->IsMounted())
        aiRegistry->GetSpellManager()->Unmount();

	if (bot->IsFriendlyTo(target))
	{
		aiRegistry->GetSocialManager()->TellMaster("Target is friendly");
		return;
	}
	if (!bot->IsWithinLOSInMap(target))
	{
		aiRegistry->GetSocialManager()->TellMaster("Target is not in my sight");
		return;
	}

	uint64 guid = target->GetGUID();
	bot->SetSelectionGuid(target->GetObjectGuid());
	bot->Attack(target, true);
	SetInFront(target);

    Pet* pet = bot->GetPet();
    if (pet)
    {
        CreatureAI* creatureAI = ((Creature*)pet)->AI();
        if (creatureAI)
            creatureAI->AttackStart(target);
    }

    aiRegistry->GetTargetManager()->SetCurrentTarget(target);
	aiRegistry->GetInventoryManager()->AddLoot(guid);
}

void AiMoveManager::ReleaseSpirit()
{
	if (bot->isAlive() || bot->GetCorpse())
		return;

	bot->SetBotDeathTimer();
	bot->BuildPlayerRepop();

	WorldLocation loc;
	Corpse *corpse = bot->GetCorpse();
	corpse->GetPosition( loc );
	bot->TeleportTo( loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, bot->GetOrientation() );
}

void AiMoveManager::Resurrect()
{
	aiRegistry->GetMoveManager()->Stay();
	Corpse* corpse = bot->GetCorpse();
	if (corpse)
	{
		time_t reclaimTime = corpse->GetGhostTime() + bot->GetCorpseReclaimDelay( corpse->GetType()==CORPSE_RESURRECTABLE_PVP );
		if (reclaimTime > time(0))
		{
			ostringstream os;
			os << "Will resurrect in ";
			os << (reclaimTime - time(0));
			os << " secs";
			aiRegistry->GetSocialManager()->TellMaster(os.str().c_str());
			ai->SetNextCheckDelay(reclaimTime - time(0));
		}
		else
			Revive();
	}
}

void AiMoveManager::Revive()
{
	PlayerbotChatHandler ch(ai->GetMaster());
	if (! ch.revive(*bot))
	{
		aiRegistry->GetSocialManager()->TellMaster(".. could not be revived ..");
		return;
	}
    aiRegistry->GetTargetManager()->SetCurrentTarget(NULL);
	bot->SetSelectionGuid(ObjectGuid());
}

void AiMoveManager::Summon()
{
    Player* master = ai->GetMaster();
    Map* masterMap = master->GetMap();
    Map* botMap = bot->GetMap();

    bool masterIsInInstance = (masterMap->IsDungeon() || masterMap->IsRaid() || masterMap->IsBattleGround());
    bool botIsInInstance = (botMap->IsDungeon() || botMap->IsRaid() || botMap->IsBattleGround());

    if ((masterIsInInstance && botIsInInstance) || (!masterIsInInstance && !botIsInInstance))
    {
        aiRegistry->GetSocialManager()->TellMaster("You can only summon me to your instance and back");
        return;
    }

    TeleportToMaster();
}

void AiMoveManager::TeleportToMaster()
{
    Player* master = bot->GetPlayerbotAI()->GetMaster();

    if (bot->IsWithinDistInMap(master, BOT_REACT_DISTANCE, true))
        return;

    PlayerbotChatHandler ch(master);
    if (!ch.teleport(*bot))
        aiRegistry->GetSocialManager()->TellMaster("You cannot summon me");
}

void AiMoveManager::UsePortal()
{
	list<GameObject*> gos = aiRegistry->GetTargetManager()->FindNearestGameObjects();
	for (list<GameObject*>::iterator i = gos.begin(); i != gos.end(); i++)
	{
		GameObject* go = *i;
		GameObjectInfo const *goInfo = go->GetGOInfo();
		if (goInfo->type != GAMEOBJECT_TYPE_SPELLCASTER)
			continue;

		uint32 spellId = goInfo->spellcaster.spellId;
		const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
		if (pSpellInfo->Effect[0] != SPELL_EFFECT_TELEPORT_UNITS && pSpellInfo->Effect[1] != SPELL_EFFECT_TELEPORT_UNITS && pSpellInfo->Effect[2] != SPELL_EFFECT_TELEPORT_UNITS)
			continue;

		ostringstream out; out << "Teleporting using " << goInfo->name;
		aiRegistry->GetSocialManager()->TellMaster(out.str().c_str());

		Spell *spell = new Spell(bot, pSpellInfo, false);
		SpellCastTargets targets;
		targets.setUnitTarget(bot);
		spell->prepare(&targets, false);
		spell->cast(true);
		return;
	}

	if (lastAreaTrigger)
	{
		WorldPacket p(CMSG_AREATRIGGER);
		p << lastAreaTrigger;
		p.rpos(0);

		bot->GetSession()->HandleAreaTriggerOpcode(p);
		lastAreaTrigger = 0;
		return;
	}

	aiRegistry->GetSocialManager()->TellMaster("Cannot find any portal to teleport");
}

void AiMoveManager::HandleCommand(const string& text, Player& fromPlayer)
{
	if (text == "attack")
	{
		Attack(ObjectAccessor::GetUnit(*bot, fromPlayer.GetSelectionGuid()));
	}
	else if(text == "release" && !bot->isAlive())
	{
		ReleaseSpirit();
	}
	else if(text == "teleport")
	{
		UsePortal();
	}
    else if(text == "fly")
    {
		list<Unit*> units = aiRegistry->GetTargetManager()->FindNearestNpcs();
		for (list<Unit*>::iterator i = units.begin(); i != units.end(); i++)
		{
			Creature *npc = bot->GetNPCIfCanInteractWith((*i)->GetObjectGuid(), UNIT_NPC_FLAG_FLIGHTMASTER);
			if (!npc)
				continue;

			if (taxiNodes.empty())
			{
				ostringstream out;
				out << "I will order a fly from " << npc->GetName() << ". Please start flying, then instruct me again to fly";
				aiRegistry->GetSocialManager()->TellMaster(out. str().c_str());
				return;
			}

			if (!bot->ActivateTaxiPathTo(taxiNodes, npc))
				aiRegistry->GetSocialManager()->TellMaster("I can not fly with you");

			return;
		}

        aiRegistry->GetSocialManager()->TellMaster("Cannot find any flightmaster to talk");
    }
    else if (text == "reset")
    {
        bot->GetMotionMaster()->Clear();
        bot->m_taxi.ClearTaxiDestinations();
    }
    else if (text == "summon")
    {
        Summon();
    }
}

void AiMoveManager::HandleBotOutgoingPacket(const WorldPacket& packet)
{
	switch (packet.GetOpcode())
	{
		// if a change in speed was detected for the master
		// make sure we have the same mount status
	case SMSG_FORCE_RUN_SPEED_CHANGE:
		{
			WorldPacket p(packet);
			uint64 guid = extractGuid(p);
			Player* master = ai->GetMaster();
			if (guid != master->GetGUID())
				return;
			if (master->IsMounted() && !bot->IsMounted())
			{
				if (!master->GetAurasByType(SPELL_AURA_MOUNTED).empty())
				{
					int32 master_speed1 = 0;
					int32 master_speed2 = 0;
					master_speed1 = ai->GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[1];
					master_speed2 = ai->GetMaster()->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetSpellProto()->EffectBasePoints[2];

					aiRegistry->GetSpellManager()->Mount(master_speed1, master_speed2);
				}
			}
			else if (!master->IsMounted() && bot->IsMounted())
			{
				aiRegistry->GetSpellManager()->Unmount();
			}
			return;
		}

		// handle flying acknowledgement
	case SMSG_MOVE_SET_CAN_FLY:
		{
			WorldPacket p(packet);
			uint64 guid = p.readPackGUID();
			if (guid != bot->GetGUID())
				return;

			bot->m_movementInfo.SetMovementFlags((MovementFlags)(MOVEFLAG_FLYING|MOVEFLAG_CAN_FLY));
			//bot->SetSpeed(MOVE_RUN, GetMaster()->GetSpeed(MOVE_FLIGHT) +0.1f, true);
			return;
		}

		// handle dismount flying acknowledgement
	case SMSG_MOVE_UNSET_CAN_FLY:
		{
			WorldPacket p(packet);
			uint64 guid = p.readPackGUID();
			if (guid != bot->GetGUID())
				return;
			bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
			//bot->SetSpeed(MOVE_RUN,GetMaster()->GetSpeedRate(MOVE_RUN),true);
			return;
		}

	case SMSG_RESURRECT_REQUEST:
		{
			if (bot->isAlive())
				return;
			WorldPacket p(packet);
			ObjectGuid guid;
			p >> guid;

			WorldPacket* const packet = new WorldPacket(CMSG_RESURRECT_RESPONSE, 8+1);
			*packet << guid;
			*packet << uint8(1);                        // accept
			bot->GetSession()->QueuePacket(packet);   // queue the packet to get around race condition
			return;
		}
	}

}

void AiMoveManager::UseMeetingStone(uint64 guid) 
{
    Player* master = bot->GetPlayerbotAI()->GetMaster();
    if (master->GetSelectionGuid().GetRawValue() != bot->GetGUID())
        return;

    GameObject* gameObject = master->GetMap()->GetGameObject(guid);
    if (!gameObject)
        return;

    const GameObjectInfo* goInfo = gameObject->GetGOInfo();
    if (!goInfo || goInfo->type != GAMEOBJECT_TYPE_SUMMONING_RITUAL)
        return;

    TeleportToMaster();
}

void AiMoveManager::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
    case CMSG_ACTIVATETAXI:
        {
            WorldPacket p(packet);
            p.rpos(0);
            
            taxiNodes.clear();
            taxiNodes.resize(2);

            p >> taxiMaster >> taxiNodes[0] >> taxiNodes[1];
            return;
        }
	case CMSG_ACTIVATETAXIEXPRESS:
		{
			WorldPacket p(packet);
			p.rpos(0);

			ObjectGuid guid;
			uint32 node_count;
			p >> guid >> node_count;

			taxiNodes.clear();
			for (uint32 i = 0; i < node_count; ++i)
			{
				uint32 node;
				p >> node;
				taxiNodes.push_back(node);
			}

			return;
		}
	case CMSG_GAMEOBJ_REPORT_USE:
        {
            WorldPacket p(packet);
            p.rpos(0);

            uint64 guid;
            p >> guid;

            UseMeetingStone(guid);
            return;
        }
	case CMSG_AREATRIGGER:
		{
			WorldPacket p(packet);
			p.rpos(0);
			p >> lastAreaTrigger;
			Player* master = bot->GetPlayerbotAI()->GetMaster();
			MoveTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ());
			
			aiRegistry->GetSocialManager()->TellMaster("Ready to teleport");
			return;
		}
    }
}

bool AiMoveManager::IsBehind(Unit* target)
{
    if (!target)
        return false;

    float targetOrientation = target->GetOrientation();
    float orientation = bot->GetOrientation();
    float distance = bot->GetDistance(target);
    
    return distance <= ATTACK_DISTANCE && abs(targetOrientation - orientation) < M_PI / 2;
}

void AiMoveManager::WaitForReach(float distance)
{
    float delay = ceil(distance / bot->GetSpeed(MOVE_RUN));
    if (delay > GLOBAL_COOLDOWN)
        delay = GLOBAL_COOLDOWN;

    bot->GetPlayerbotAI()->SetNextCheckDelay((uint32)delay);
}

void AiMoveManager::Update()
{
	if (!bot->GetAurasByType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED).empty())
	{
		bot->m_movementInfo.SetMovementFlags((MovementFlags)(MOVEFLAG_FLYING|MOVEFLAG_CAN_FLY));

		WorldPacket packet(CMSG_MOVE_SET_FLY);
		packet << bot->GetObjectGuid().WriteAsPacked();
		packet << bot->m_movementInfo;
		bot->SetMover(bot);
		bot->GetSession()->HandleMovementOpcodes(packet);
	}

	if (bot->IsMounted() && bot->IsFlying())
	{
		bot->m_movementInfo.SetMovementFlags((MovementFlags)(MOVEFLAG_FLYING|MOVEFLAG_CAN_FLY));
		bot->SetSpeedRate(MOVE_FLIGHT, 1.0f, true);
		bot->SetSpeedRate(MOVE_FLIGHT, ai->GetMaster()->GetSpeedRate(MOVE_FLIGHT), true);

		bot->SetSpeedRate(MOVE_RUN, 1.0f, true);
		bot->SetSpeedRate(MOVE_RUN, ai->GetMaster()->GetSpeedRate(MOVE_FLIGHT), true);
	}
}