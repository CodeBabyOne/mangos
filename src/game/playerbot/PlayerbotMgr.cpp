#include "Config/Config.h"
#include "../pchdef.h"
#include "playerbot.h"

class LoginQueryHolder;
class CharacterHandler;


PlayerbotMgr::PlayerbotMgr(Player* const master) : PlayerbotAIBase(),  m_master(master) 
{
    // load config variables
	m_confDisableBots = sConfig.GetBoolDefault( "PlayerbotAI.DisableBots", false );
    m_confDebugWhisper = sConfig.GetBoolDefault( "PlayerbotAI.DebugWhisper", false );
    m_confFollowDistance[0] = sConfig.GetFloatDefault( "PlayerbotAI.FollowDistanceMin", 0.5f );
    m_confFollowDistance[1] = sConfig.GetFloatDefault( "PlayerbotAI.FollowDistanceMin", 1.0f );

    groupStatsManager = new ai::AiGroupStatsManager(master);
}

PlayerbotMgr::~PlayerbotMgr() 
{
    LogoutAllBots();
    if (groupStatsManager)
        delete groupStatsManager;
}

void PlayerbotMgr::UpdateAI(const uint32 p_time) 
{
    if (!CanUpdateAI())
        return;

    SetNextCheckDelay(GLOBAL_COOLDOWN);

    groupStatsManager->Update();
}

void PlayerbotMgr::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        bot->GetPlayerbotAI()->HandleMasterIncomingPacket(packet);
    }

    switch (packet.GetOpcode())
    {
        // if master is logging out, log out all bots
        case CMSG_LOGOUT_REQUEST:
        {
            LogoutAllBots();
            return;
        }

        // If master inspects one of his bots, give the master useful info in chat window
        // such as inventory that can be equipped
        case CMSG_INSPECT:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint64 guid;
            p >> guid;
            Player* const bot = GetPlayerBot(guid);
            //if (bot) bot->GetPlayerbotAI()->SendNotEquipList(*bot);
            return;
        }

        case CMSG_GAMEOBJ_USE:
		case CMSG_GAMEOBJ_REPORT_USE:
            {
                WorldPacket p(packet);
        	    p.rpos(0); // reset reader
        	    uint64 objGUID;
        	    p >> objGUID;

                GameObject *obj = m_master->GetMap()->GetGameObject( objGUID );
                if( !obj )
                    return;

            	for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
            	{
            		Player* const bot = it->second;

                    if( obj->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER )
                    {
                        bot->GetPlayerbotAI()->GetAiRegistry()->GetQuestManager()->TurnInQuests( obj );
                    }
                    // add other go types here, i.e.:
                    // GAMEOBJECT_TYPE_CHEST - loot quest items of chest
                }
            }
            break;

        // if master talks to an NPC
        case CMSG_GOSSIP_HELLO:
        case CMSG_QUESTGIVER_HELLO:
        {
        	WorldPacket p(packet);
        	p.rpos(0); // reset reader
        	uint64 npcGUID;
        	p >> npcGUID;
        	
        	WorldObject* pNpc = m_master->GetMap()->GetWorldObject( npcGUID );
        	if (!pNpc)
        		return;

        	// for all master's bots
        	for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
        	{
        		Player* const bot = it->second;
                bot->GetPlayerbotAI()->GetAiRegistry()->GetQuestManager()->TurnInQuests( pNpc );
        	}
        	        
        	return;
        }

        // if master accepts a quest, bots should also try to accept quest
        case CMSG_QUESTGIVER_ACCEPT_QUEST:
        {
            WorldPacket p(packet);
            p.rpos(0); // reset reader
            uint64 guid;
            uint32 quest;
            p >> guid >> quest;
            Quest const* qInfo = sObjectMgr.GetQuestTemplate(quest);
            if (qInfo)
            {
                for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
                {
                    Player* const bot = it->second;
                    
                    if (bot->GetQuestStatus(quest) == QUEST_STATUS_COMPLETE)
						bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("I already completed that quest.");
                    else if (! bot->CanTakeQuest(qInfo, false))
                    {                    	
        				if (! bot->SatisfyQuestStatus(qInfo, false))
                            bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("I already have that quest.");
                        else
                            bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("I can't take that quest.");
                    }
                    else if (! bot->SatisfyQuestLog(false))
                        bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("My quest log is full.");
                    else if (! bot->CanAddQuest(qInfo, false))
                        bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("I can't take that quest because it requires that I take items, but my bags are full!");

                    else
                    {
                        p.rpos(0); // reset reader
                        bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);
                        bot->GetPlayerbotAI()->GetAiRegistry()->GetSocialManager()->TellMaster("Got the quest.");
                    }
                }
            }
            return;
        }
		case CMSG_LOOT_ROLL:
		{

			WorldPacket p(packet); //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
			ObjectGuid Guid;
			uint32 NumberOfPlayers;
			uint8 rollType;
			p.rpos(0); //reset packet pointer
			p >> Guid; //guid of the item rolled
			p >> NumberOfPlayers; //number of players invited to roll
			p >> rollType; //need,greed or pass on roll


			for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
				{

					uint32 choice = urand(0,2); //returns 0,1,or 2

					Player* const bot = it->second;
					if(!bot)
						return;

					Group* group = bot->GetGroup();
					if(!group)
						return;

					switch (group->GetLootMethod())
						{
						case GROUP_LOOT:
							// bot random roll
							group->CountRollVote(bot, Guid, NumberOfPlayers, ROLL_NEED);
							break;
						case NEED_BEFORE_GREED:
							choice = 1;
							// bot need roll
							group->CountRollVote(bot, Guid, NumberOfPlayers, ROLL_NEED);
							break;
						case MASTER_LOOT:
							choice = 0;
							// bot pass on roll
							group->CountRollVote(bot, Guid, NumberOfPlayers, ROLL_PASS);
							break;
						default:
							break;
						}

					switch (rollType)
						{
						case ROLL_NEED:
							bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_NEED, 1);
							break;
						case ROLL_GREED:
							bot->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_ROLL_GREED, 1);
							break;
						}

				}
		return;
		}

		case CMSG_REPAIR_ITEM:
			{

				WorldPacket p(packet); // WorldPacket packet for CMSG_REPAIR_ITEM, (8+8+1)

				sLog.outDebug("PlayerbotMgr: CMSG_REPAIR_ITEM");

				uint64 npcGUID, itemGUID;
				uint8 guildBank;

				p.rpos(0); //reset packet pointer
				p >> npcGUID;
				p >> itemGUID;  // Not used for bot but necessary opcode data retrieval
				p >> guildBank; // Flagged if guild repair selected

				for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
				{

					Player* const bot = it->second;
					if(!bot)
						return;

					Group* group = bot->GetGroup();  // check if bot is a member of group
					if(!group)
						return;

					Creature *unit = bot->GetNPCIfCanInteractWith(npcGUID, UNIT_NPC_FLAG_REPAIR);
					if (!unit) // Check if NPC can repair bot or not
						return;

					// remove fake death
					if(bot->hasUnitState(UNIT_STAT_DIED))
						bot->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

					// reputation discount
					float discountMod = bot->GetReputationPriceDiscount(unit);

					uint32 TotalCost = 0;
					if (itemGUID) // Handle redundant feature (repair individual item) for bot
					{
						sLog.outDebug("ITEM: Repair single item is not applicable for %s",bot->GetName());
						continue;
					}
					else  // Handle feature (repair all items) for bot
					{
						TotalCost = bot->DurabilityRepairAll(true,discountMod,guildBank>0?true:false);
					}
					if (guildBank) // Handle guild repair
					{
						uint32 GuildId = bot->GetGuildId();
						if (!GuildId)
							return;
						Guild *pGuild = sGuildMgr.GetGuildById(GuildId);
						if (!pGuild)
							return;
						pGuild->LogBankEvent(GUILD_BANK_LOG_REPAIR_MONEY, 0, bot->GetGUIDLow(), TotalCost);
						pGuild->SendMoneyInfo(bot->GetSession(), bot->GetGUIDLow());
					}

				}
				return;
			}

        /*
        case CMSG_NAME_QUERY:
        case MSG_MOVE_START_FORWARD:
        case MSG_MOVE_STOP:
        case MSG_MOVE_SET_FACING:
        case MSG_MOVE_START_STRAFE_LEFT:
        case MSG_MOVE_START_STRAFE_RIGHT:
        case MSG_MOVE_STOP_STRAFE:
        case MSG_MOVE_START_BACKWARD:
        case MSG_MOVE_HEARTBEAT:
        case CMSG_STANDSTATECHANGE:
        case CMSG_QUERY_TIME:
        case CMSG_CREATURE_QUERY:
        case CMSG_GAMEOBJECT_QUERY:
        case MSG_MOVE_JUMP:
        case MSG_MOVE_FALL_LAND:
            return;

        default:
        {
            const char* oc = LookupOpcodeName(packet.GetOpcode());
            // ChatHandler ch(m_master);
            // ch.SendSysMessage(oc);

            std::ostringstream out;
            out << "masterin: " << oc;
            sLog.outError(out.str().c_str());
        }
        */
    }
}
void PlayerbotMgr::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
}

void PlayerbotMgr::LogoutAllBots()
{
    while (true)
    {
        PlayerBotMap::const_iterator itr = GetPlayerBotsBegin();
        if (itr == GetPlayerBotsEnd()) break;
        Player* bot= itr->second;
        LogoutPlayerBot(bot->GetGUID());
    }
}

// Playerbot mod: logs out a Playerbot.
void PlayerbotMgr::LogoutPlayerBot(uint64 guid)
{
    Player* bot= GetPlayerBot(guid);
    if (bot)
    {
        WorldSession * botWorldSessionPtr = bot->GetSession();
        m_playerBots.erase(guid);    // deletes bot player ptr inside this WorldSession PlayerBotMap
        botWorldSessionPtr->LogoutPlayer(true); // this will delete the bot Player object and PlayerbotAI object
        delete botWorldSessionPtr;  // finally delete the bot's WorldSession
    }
}

// Playerbot mod: Gets a player bot Player object for this WorldSession master
Player* PlayerbotMgr::GetPlayerBot(uint64 playerGuid) const
{
    PlayerBotMap::const_iterator it = m_playerBots.find(playerGuid);
    return (it == m_playerBots.end()) ? 0 : it->second;
}

void PlayerbotMgr::OnBotLogin(Player * const bot)
{
    // give the bot some AI, object is owned by the player class
    PlayerbotAI* ai = new PlayerbotAI(this, bot);
    bot->SetPlayerbotAI(ai);

    // tell the world session that they now manage this new bot
    m_playerBots[bot->GetGUID()] = bot;

    // sometimes master can lose leadership, pass leadership to master check
    const uint64 masterGuid = m_master->GetGUID();
    if (m_master->GetGroup() && 
        ! m_master->GetGroup()->IsLeader(masterGuid))
        m_master->GetGroup()->ChangeLeader(masterGuid);
}

bool processBotCommand(WorldSession* session, string cmdStr, uint64 guid)
{
    if (guid == 0 || (guid == session->GetPlayer()->GetGUID()))
        return false;

    PlayerbotMgr* mgr = session->GetPlayer()->GetPlayerbotMgr();

    if (cmdStr == "add" || cmdStr == "login")
    {
        if (mgr->GetPlayerBot(guid)) 
            return false;
        
        mgr->AddPlayerBot(guid, session);
    }
    else if (cmdStr == "remove" || cmdStr == "logout")
    {
        if (! mgr->GetPlayerBot(guid))
            return false;
        
        mgr->LogoutPlayerBot(guid);
    }

    return true;
}

bool ChatHandler::HandlePlayerbotCommand(char* args)
{
	if(sConfig.GetBoolDefault("PlayerbotAI.DisableBots", false))
	{
		PSendSysMessage("|cffff0000Playerbot system is currently disabled!");
        SetSentErrorMessage(true);
        return false;
	}

    if (! m_session)
    {
        PSendSysMessage("You may only add bots from an active session");
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    char *cmd = strtok ((char*)args, " ");
    char *charname = strtok (NULL, " ");
    if (!cmd || !charname)
    {
        PSendSysMessage("usage: add PLAYERNAME  or  remove PLAYERNAME");
        SetSentErrorMessage(true);
        return false;
    }

    std::string cmdStr = cmd;
    std::string charnameStr = charname;

    Player* player = m_session->GetPlayer();

    // create the playerbot manager if it doesn't already exist
    PlayerbotMgr* mgr = player->GetPlayerbotMgr();
    if (!mgr)
    {
        mgr = new PlayerbotMgr(player);
        player->SetPlayerbotMgr(mgr);
    }

    if (charnameStr == "*")
    {
        Group* group = player->GetGroup();
        if (!group)
        {
            PSendSysMessage("you must be in group");
            SetSentErrorMessage(true);
            return false;
        }

        bool res = true;
        Group::MemberSlotList slots = group->GetMemberSlots();
        for (Group::member_citerator i = slots.begin(); i != slots.end(); i++) 
        {
            uint64 member = i->guid.GetRawValue();
            if (member != player->GetGUID() && !processBotCommand(m_session, cmdStr, member))
            {
                PSendSysMessage("Error processing bot command");
                SetSentErrorMessage(true);
                res = false;
            }
        }
        return res;
    }

    bool res = true;
    vector<string> chars = split(charnameStr, ',');
    for (vector<string>::iterator i = chars.begin(); i != chars.end(); i++)
    {
        string s = *i;
        res &= processBotCommand(m_session, cmdStr, sObjectMgr.GetPlayerGUIDByName(s.c_str()));
        if (!res)
        {
            PSendSysMessage("Error processing bot command");
            SetSentErrorMessage(true);
        }
    }
    return res;
}
