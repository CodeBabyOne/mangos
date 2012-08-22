#include "../pchdef.h"
#include "PlayerbotMgr.h"
#include "playerbot.h"

#include "AiFactory.h"

#include "../GridNotifiers.h"
#include "../GridNotifiersImpl.h"
#include "../CellImpl.h"
#include "strategy/values/LastMovementValue.h"
#include "strategy/actions/LogLevelAction.h"
#include "strategy/values/LastSpellCastValue.h"
#include "LootObjectStack.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotAI.h"
#include "PlayerbotFactory.h"

using namespace ai;
using namespace std;

vector<string>& split(const string &s, char delim, vector<string> &elems);
vector<string> split(const string &s, char delim);
char * strstri (string str1, string str2);
uint64 extractGuid(WorldPacket& packet);
std::string &trim(std::string &s);

uint32 PlayerbotChatHandler::extractQuestId(string str)
{
    char* source = (char*)str.c_str();
    char* cId = ExtractKeyFromLink(&source,"Hquest");
    return cId ? atol(cId) : 0;
}

void PacketHandlingHelper::AddHandler(uint16 opcode, string handler)
{
    handlers[opcode] = handler;
}

void PacketHandlingHelper::Handle(ExternalEventHelper &helper)
{
    while (!queue.empty())
    {
        helper.HandlePacket(handlers, queue.top());
        queue.pop();
    }
}

void PacketHandlingHelper::AddPacket(const WorldPacket& packet)
{
    queue.push(packet);
}


PlayerbotAI::PlayerbotAI() : PlayerbotAIBase(), bot(NULL), mgr(NULL), aiObjectContext(NULL),
    currentEngine(NULL), chatHelper(this), chatFilter(this)
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
        engines[i] = NULL;
}

PlayerbotAI::PlayerbotAI(PlayerbotMgr* mgr, Player* bot, NamedObjectContext<UntypedValue>* sharedValues) :
    PlayerbotAIBase(), chatHelper(this), chatFilter(this)
{
	this->mgr = mgr;
	this->bot = bot;

	accountId = sObjectMgr.GetPlayerAccountIdByGUID(bot->GetObjectGuid());

    aiObjectContext = AiFactory::createAiObjectContext(bot, this);
    aiObjectContext->AddShared(sharedValues);

    engines[BOT_STATE_COMBAT] = AiFactory::createCombatEngine(bot, this, aiObjectContext);
    engines[BOT_STATE_NON_COMBAT] = AiFactory::createNonCombatEngine(bot, this, aiObjectContext);
    engines[BOT_STATE_DEAD] = AiFactory::createDeadEngine(bot, this, aiObjectContext);
    currentEngine = engines[BOT_STATE_NON_COMBAT];

    masterIncomingPacketHandlers.AddHandler(CMSG_GAMEOBJ_REPORT_USE, "use game object");
    masterIncomingPacketHandlers.AddHandler(CMSG_AREATRIGGER, "area trigger");
    masterIncomingPacketHandlers.AddHandler(CMSG_GAMEOBJ_USE, "use game object");
    masterIncomingPacketHandlers.AddHandler(CMSG_LOOT_ROLL, "loot roll");

    masterIncomingPacketHandlers.AddHandler(CMSG_GOSSIP_HELLO, "gossip hello");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_HELLO, "gossip hello");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_COMPLETE_QUEST, "complete quest");
    masterIncomingPacketHandlers.AddHandler(CMSG_QUESTGIVER_ACCEPT_QUEST, "accept quest");

    masterIncomingPacketHandlers.AddHandler(CMSG_ACTIVATETAXI, "activate taxi");
    masterIncomingPacketHandlers.AddHandler(CMSG_ACTIVATETAXIEXPRESS, "activate taxi");
    masterIncomingPacketHandlers.AddHandler(CMSG_MOVE_SPLINE_DONE, "taxi done");

    botOutgoingPacketHandlers.AddHandler(SMSG_QUESTGIVER_QUEST_DETAILS, "quest share");
    botOutgoingPacketHandlers.AddHandler(SMSG_GROUP_INVITE, "group invite");
    botOutgoingPacketHandlers.AddHandler(BUY_ERR_NOT_ENOUGHT_MONEY, "not enough money");
    botOutgoingPacketHandlers.AddHandler(BUY_ERR_REPUTATION_REQUIRE, "not enough reputation");
    botOutgoingPacketHandlers.AddHandler(SMSG_GROUP_SET_LEADER, "group set leader");
    botOutgoingPacketHandlers.AddHandler(SMSG_FORCE_RUN_SPEED_CHANGE, "check mount state");
    botOutgoingPacketHandlers.AddHandler(SMSG_RESURRECT_REQUEST, "resurrect request");
    botOutgoingPacketHandlers.AddHandler(SMSG_INVENTORY_CHANGE_FAILURE, "cannot equip");
    botOutgoingPacketHandlers.AddHandler(SMSG_TRADE_STATUS, "trade status");
    botOutgoingPacketHandlers.AddHandler(SMSG_LOOT_RESPONSE, "loot response");
    botOutgoingPacketHandlers.AddHandler(SMSG_QUESTUPDATE_ADD_KILL, "quest objective completed");
    botOutgoingPacketHandlers.AddHandler(SMSG_ITEM_PUSH_RESULT, "item push result");
    botOutgoingPacketHandlers.AddHandler(SMSG_PARTY_COMMAND_RESULT, "party command");
    botOutgoingPacketHandlers.AddHandler(SMSG_CAST_FAILED, "cast failed");
    botOutgoingPacketHandlers.AddHandler(SMSG_DUEL_REQUESTED, "duel requested");

    masterOutgoingPacketHandlers.AddHandler(SMSG_PARTY_COMMAND_RESULT, "party command");
    masterOutgoingPacketHandlers.AddHandler(MSG_RAID_READY_CHECK, "ready check");
    masterOutgoingPacketHandlers.AddHandler(MSG_RAID_READY_CHECK_FINISHED, "ready check finished");
}

PlayerbotAI::~PlayerbotAI()
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
    {
        if (engines[i])
            delete engines[i];
    }

    if (aiObjectContext)
        delete aiObjectContext;
}

void PlayerbotAI::UpdateAI(uint32 elapsed)
{
    if (bot->IsBeingTeleported())
        return;

    ChangeActiveEngineIfNecessary();

    PlayerbotAIBase::UpdateAI(elapsed);
}

void PlayerbotAI::UpdateAIInternal(uint32 elapsed)
{
    ExternalEventHelper helper(aiObjectContext);
    while (!chatCommands.empty())
    {
        string command = chatCommands.top();
        if (!helper.ParseChatCommand(command))
        {
            ostringstream out; out << "Unknown command " << command;
            TellMaster(out);
            helper.ParseChatCommand("help");
        }
        chatCommands.pop();
    }

    botOutgoingPacketHandlers.Handle(helper);
    masterIncomingPacketHandlers.Handle(helper);
    masterOutgoingPacketHandlers.Handle(helper);

	DoNextAction();
}

void PlayerbotAI::HandleTeleportAck()
{
	bot->GetMotionMaster()->Clear(true);
	if (bot->IsBeingTeleportedNear())
	{
		WorldPacket p = WorldPacket(MSG_MOVE_TELEPORT_ACK, 8 + 4 + 4);
		p.appendPackGUID(bot->GetObjectGuid().GetRawValue());
		p << (uint32) 0; // supposed to be flags? not used currently
		p << (uint32) time(0); // time - not currently used
		bot->GetSession()->HandleMoveTeleportAckOpcode(p);
	}
	else if (bot->IsBeingTeleportedFar())
		bot->GetSession()->HandleMoveWorldportAckOpcode();
}

void PlayerbotAI::Reset()
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
            engines[i]->Init();
    currentEngine = engines[BOT_STATE_NON_COMBAT];
    nextAICheckDelay = 0;
    aiObjectContext->GetValue<Unit*>("current target")->Set(NULL);
    aiObjectContext->GetValue<LootObject>("loot target")->Set(LootObject());
    LastSpellCast & lastSpell = aiObjectContext->GetValue<LastSpellCast& >("last spell cast")->Get();
    lastSpell.Reset();
    LastMovement & lastMovement = aiObjectContext->GetValue<LastMovement& >("last movement")->Get();
    lastMovement.Set(NULL);
    bot->GetMotionMaster()->Clear();
    bot->m_taxi.ClearTaxiDestinations();
    ResetStrategies();
}

void PlayerbotAI::HandleCommand(uint32 type, const string& text, Player& fromPlayer)
{
    if (fromPlayer.GetObjectGuid() != bot->GetPlayerbotAI()->GetMaster()->GetObjectGuid() || fromPlayer.GetPlayerbotAI())
        return;

    for (string::const_iterator i = text.begin(); i != text.end(); i++)
    {
        char symbol = *i;
        if (symbol < 32 || symbol > 127)
            return;
    }

    if (type == CHAT_MSG_RAID_WARNING && text.find(bot->GetName()) != string::npos && text.find("award") == string::npos)
    {
        chatCommands.push("warning");
        return;
    }

	// ignore any messages from Addons
	if (text.empty() ||
		text.find("X-Perl") != wstring::npos ||
		text.find("HealBot") != wstring::npos ||
		text.find("LOOT_OPENED") != wstring::npos ||
		text.find("CTRA") != wstring::npos)
		return;

	string filtered = chatFilter.Filter(trim((string&)text));
	if (filtered.empty())
	    return;

    if (filtered.size() > 2 && filtered.substr(0, 2) == "d " || filtered.size() > 3 && filtered.substr(0, 3) == "do ")
    {
        std::string action = filtered.substr(filtered.find(" ") + 1);
        DoSpecificAction(action);
    }
    else if (filtered == "reset")
    {
        Reset();
    }
    else
    {
        chatCommands.push(filtered);
    }
}

void PlayerbotAI::HandleBotOutgoingPacket(const WorldPacket& packet)
{
    switch (packet.GetOpcode())
    {
    case SMSG_MOVE_SET_CAN_FLY:
        {
            WorldPacket p(packet);
            uint64 guid = p.readPackGUID();
            if (guid != bot->GetObjectGuid().GetRawValue())
                return;

            bot->m_movementInfo.SetMovementFlags((MovementFlags)(MOVEFLAG_FLYING|MOVEFLAG_CAN_FLY));
            return;
        }
    case SMSG_MOVE_UNSET_CAN_FLY:
        {
            WorldPacket p(packet);
            uint64 guid = p.readPackGUID();
            if (guid != bot->GetObjectGuid().GetRawValue())
                return;
            bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            return;
        }
    case SMSG_CAST_FAILED:
        {
            WorldPacket p(packet);
            p.rpos(0);
            uint8 castCount, result;
            uint32 spellId;
            p >> castCount >> spellId >> result;
            if (result != SPELL_CAST_OK)
            {
                SpellInterrupted(spellId);
                botOutgoingPacketHandlers.AddPacket(packet);
            }
            return;
        }
    case SMSG_SPELL_FAILURE:
        {
            WorldPacket p(packet);
            p.rpos(0);
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();
            if (casterGuid != bot->GetObjectGuid().GetRawValue())
                return;

            uint8 castCount;
            uint32 spellId;
            p >> castCount;
            p >> spellId;
            SpellInterrupted(spellId);
            return;
        }
    case SMSG_SPELL_DELAYED:
        {
            WorldPacket p(packet);
            p.rpos(0);
            ObjectGuid casterGuid;
            p >> casterGuid.ReadAsPacked();

            if (casterGuid != bot->GetObjectGuid())
                return;

            uint32 delaytime;
            p >> delaytime;
            if (delaytime <= 1000)
                IncreaseNextCheckDelay(delaytime);
            return;
        }
    default:
        botOutgoingPacketHandlers.AddPacket(packet);
    }
}

void PlayerbotAI::SpellInterrupted(uint32 spellid)
{
    LastSpellCast& lastSpell = aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get();
    if (lastSpell.id != spellid)
        return;

    lastSpell.Reset();

    time_t now = time(0);
    if (now <= lastSpell.time)
        return;

    uint32 castTimeSpent = 1000 * (now - lastSpell.time);

    int32 globalCooldown = CalculateGlobalCooldown(lastSpell.id);
    if (castTimeSpent < globalCooldown)
        SetNextCheckDelay(globalCooldown - castTimeSpent);
    else
        SetNextCheckDelay(0);

    lastSpell.id = 0;
}

int32 PlayerbotAI::CalculateGlobalCooldown(uint32 spellid)
{
    if (!spellid)
        return 0;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellid );

    if (bot->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
        return sPlayerbotAIConfig.globalCoolDown;

    return sPlayerbotAIConfig.reactDelay;
}

void PlayerbotAI::HandleMasterIncomingPacket(const WorldPacket& packet)
{
    masterIncomingPacketHandlers.AddPacket(packet);
}

void PlayerbotAI::HandleMasterOutgoingPacket(const WorldPacket& packet)
{
    masterOutgoingPacketHandlers.AddPacket(packet);
}

void PlayerbotAI::ChangeActiveEngineIfNecessary()
{
	Player* master = GetMaster();
    if (!bot->isAlive())
    {
        ChangeEngine(BOT_STATE_DEAD);

        if (!bot->IsFriendlyTo(master) && !bot->GetCorpse())
        {
            bot->SetBotDeathTimer();
            bot->BuildPlayerRepop();
            Corpse *corpse = bot->GetCorpse();
            WorldLocation loc;
            corpse->GetPosition( loc );
            bot->TeleportTo( loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, bot->GetOrientation() );
        }
        return;
    }

    Unit* target = aiObjectContext->GetValue<Unit*>("current target")->Get();
    if (bot->duel && !target) {
        aiObjectContext->GetValue<Unit*>("old target")->Set(target);

        Player* target = bot->duel->opponent;
        aiObjectContext->GetValue<Unit*>("old target")->Set(target);
        bot->SetSelectionGuid(target->GetObjectGuid());
        ChangeEngine(BOT_STATE_COMBAT);
    }
    else if (target && target->isAlive() && target->IsHostileTo(bot))
    {
        ChangeEngine(BOT_STATE_COMBAT);
    }
    else
    {
        aiObjectContext->GetValue<Unit*>("current target")->Set(NULL);
        bot->SetSelectionGuid(ObjectGuid());
        ChangeEngine(BOT_STATE_NON_COMBAT);
    }
}

void PlayerbotAI::ChangeEngine(BotState type)
{
    Engine* engine = engines[type];

    if (currentEngine != engine)
    {
        currentEngine = engine;
        ReInitCurrentEngine();

        switch (type)
        {
        case BOT_STATE_COMBAT:
            sLog.outDebug("=== %s COMBAT ===", bot->GetName());
            break;
        case BOT_STATE_NON_COMBAT:
            sLog.outDebug("=== %s NON-COMBAT ===", bot->GetName());
            break;
        case BOT_STATE_DEAD:
            sLog.outDebug("=== %s DEAD ===", bot->GetName());
            break;
        }
    }
}

void PlayerbotAI::DoNextAction()
{
    if (bot->IsBeingTeleported())
        return;

    Player* master = GetMaster();
    if (bot->GetMapId() == master->GetMapId())
    {
        bot->UpdateUnderwaterState(bot->GetMap(), bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());
        bot->CheckAreaExploreAndOutdoor();
    }

    currentEngine->DoNextAction(NULL);

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
        bot->SetSpeedRate(MOVE_FLIGHT, GetMaster()->GetSpeedRate(MOVE_FLIGHT), true);

        bot->SetSpeedRate(MOVE_RUN, 1.0f, true);
        bot->SetSpeedRate(MOVE_RUN, GetMaster()->GetSpeedRate(MOVE_FLIGHT), true);
    }
}

void PlayerbotAI::ReInitCurrentEngine()
{
    InterruptSpell();
    currentEngine->Init();
}

void PlayerbotAI::ChangeStrategy(string names, BotState type)
{
    Engine* e = engines[type];
    if (!e)
        return;

    vector<string> splitted = split(names, ',');
    for (vector<string>::iterator i = splitted.begin(); i != splitted.end(); i++)
    {
        const char* name = i->c_str();
        switch (name[0])
        {
        case '+':
            e->addStrategy(name+1);
            break;
        case '-':
            e->removeStrategy(name+1);
            break;
        case '~':
            e->toggleStrategy(name+1);
            break;
        case '?':
            TellMaster(e->ListStrategies());
            break;
        }
    }
}

void PlayerbotAI::DoSpecificAction(string name)
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
    {
        if (engines[i]->ExecuteAction(name))
            return;
    }

    ostringstream out;
    out << "I cannot do ";
    out << name;
    TellMaster(out.str());
}

bool PlayerbotAI::ContainsStrategy(StrategyType type)
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
    {
        if (engines[i]->ContainsStrategy(type))
            return true;
    }
    return false;
}

bool PlayerbotAI::HasStrategy(string name, BotState type)
{
    return engines[type]->HasStrategy(name);
}

void PlayerbotAI::ResetStrategies()
{
    for (int i = 0 ; i < BOT_STATE_MAX; i++)
        engines[i]->removeAllStrategies();

    AiFactory::AddDefaultCombatStrategies(bot, this, engines[BOT_STATE_COMBAT]);
    AiFactory::AddDefaultNonCombatStrategies(bot, this, engines[BOT_STATE_NON_COMBAT]);
    AiFactory::AddDefaultDeadStrategies(bot, this, engines[BOT_STATE_DEAD]);
}

bool PlayerbotAI::IsTank(Player* player)
{
    PlayerbotAI* botAi = player->GetPlayerbotAI();
    if (botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_TANK);

    switch (player->getClass())
    {
    case CLASS_DEATH_KNIGHT:
    case CLASS_PALADIN:
    case CLASS_WARRIOR:
        return true;
    case CLASS_DRUID:
        return HasAnyAuraOf(player, "bear form", "dire bear form", NULL);
    }
    return false;
}

bool PlayerbotAI::IsHeal(Player* player)
{
    PlayerbotAI* botAi = player->GetPlayerbotAI();
    if (botAi)
        return botAi->ContainsStrategy(STRATEGY_TYPE_HEAL);

    switch (player->getClass())
    {
    case CLASS_PRIEST:
        return true;
    case CLASS_DRUID:
        return HasAnyAuraOf(player, "tree of life form", NULL);
    }
    return false;
}



namespace MaNGOS
{

    class UnitByGuidInRangeCheck
    {
    public:
        UnitByGuidInRangeCheck(WorldObject const* obj, ObjectGuid guid, float range) : i_obj(obj), i_range(range), i_guid(guid) {}
        WorldObject const& GetFocusObject() const { return *i_obj; }
        bool operator()(Unit* u)
        {
            return u->GetObjectGuid() == i_guid && i_obj->IsWithinDistInMap(u, i_range);
        }
    private:
        WorldObject const* i_obj;
        float i_range;
        ObjectGuid i_guid;
    };

    class GameObjectByGuidInRangeCheck
    {
    public:
        GameObjectByGuidInRangeCheck(WorldObject const* obj, ObjectGuid guid, float range) : i_obj(obj), i_range(range), i_guid(guid) {}
        WorldObject const& GetFocusObject() const { return *i_obj; }
        bool operator()(GameObject* u)
        {
            if (u && i_obj->IsWithinDistInMap(u, i_range) && u->isSpawned() && u->GetGOInfo() && u->GetObjectGuid() == i_guid)
                return true;

            return false;
        }
    private:
        WorldObject const* i_obj;
        float i_range;
        ObjectGuid i_guid;
    };

};


Unit* PlayerbotAI::GetUnit(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    if (bot->GetMapId() != GetMaster()->GetMapId())
        return NULL;

    list<Unit*> targets;

    MaNGOS::UnitByGuidInRangeCheck u_check(bot, guid, sPlayerbotAIConfig.sightDistance);
    MaNGOS::UnitListSearcher<MaNGOS::UnitByGuidInRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, sPlayerbotAIConfig.sightDistance);

    if (targets.empty())
        return NULL;

    return *targets.begin();
}


Creature* PlayerbotAI::GetCreature(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    if (bot->GetMapId() != GetMaster()->GetMapId())
        return NULL;

    list<Unit *> targets;

    MaNGOS::UnitByGuidInRangeCheck u_check(bot, guid, sPlayerbotAIConfig.sightDistance);
    MaNGOS::UnitListSearcher<MaNGOS::UnitByGuidInRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, sPlayerbotAIConfig.sightDistance);

    for(list<Unit *>::iterator i = targets.begin(); i != targets.end(); i++)
    {
        Creature* creature = dynamic_cast<Creature*>(*i);
        if (creature)
            return creature;
    }

    return NULL;
}

GameObject* PlayerbotAI::GetGameObject(ObjectGuid guid)
{
    if (!guid)
        return NULL;

    if (bot->GetMapId() != GetMaster()->GetMapId())
        return NULL;

    list<GameObject*> targets;

    MaNGOS::GameObjectByGuidInRangeCheck u_check(bot, guid, sPlayerbotAIConfig.sightDistance);
    MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectByGuidInRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, sPlayerbotAIConfig.sightDistance);

    for(list<GameObject*>::iterator i = targets.begin(); i != targets.end(); i++)
    {
        GameObject* go = *i;
        if (go && go->isSpawned())
            return go;
    }

    return NULL;
}

void PlayerbotAI::TellMaster(string text)
{
    LogLevel logLevel = *aiObjectContext->GetValue<LogLevel>("log level");
    if (IsOpposing(GetMaster()) && logLevel != LOG_LVL_DEBUG)
        return;

    if (sPlayerbotAIConfig.IsInRandomAccountList(accountId) && logLevel != LOG_LVL_DEBUG && !bot->GetGroup())
        return;

    WorldPacket data(SMSG_MESSAGECHAT, 1024);
    bot->BuildPlayerChat(&data, *aiObjectContext->GetValue<ChatMsg>("chat"), text, LANG_UNIVERSAL);
    GetMaster()->GetSession()->SendPacket(&data);
    bot->HandleEmoteCommand(EMOTE_ONESHOT_TALK);

}

void PlayerbotAI::TellMaster(LogLevel level, string text)
{
    LogLevel logLevel = *aiObjectContext->GetValue<LogLevel>("log level");

    if (logLevel < level)
        return;

    ostringstream out;
    out << LogLevelAction::logLevel2string(level) << ": " << text;
    TellMaster(out.str());
}


bool IsRealAura(Player* bot, Aura* aura, Unit* unit)
{
    if (!aura)
        return false;

    if (!unit->IsHostileTo(bot))
        return true;

    uint32 stacks = aura->GetHolder()->GetStackAmount();
    if (stacks >= aura->GetSpellProto()->StackAmount)
        return true;

    if (aura->GetCaster() == bot || aura->IsPositive() || aura->IsAreaAura())
        return true;

    return false;
}

bool PlayerbotAI::HasAura(string name, Unit* unit)
{
    if (!unit)
        return false;

    wstring wnamepart;
    if (!Utf8toWStr(name, wnamepart))
        return 0;

    wstrToLower(wnamepart);

    for (uint32 auraType = SPELL_AURA_BIND_SIGHT; auraType < TOTAL_AURAS; auraType++)
    {
        Unit::AuraList auras = unit->GetAurasByType((AuraType)auraType);
        for (Unit::AuraList::iterator i = auras.begin(); i != auras.end(); i++)
        {
            Aura* aura = *i;
            const string auraName = aura->GetSpellProto()->SpellName[0];
            if (auraName.empty() || auraName.length() != wnamepart.length() || !Utf8FitTo(auraName, wnamepart))
                continue;

            if (IsRealAura(bot, aura, unit))
                return true;
        }
    }

    return HasAura(aiObjectContext->GetValue<uint32>("spell id", name)->Get(), unit);
}

bool PlayerbotAI::HasAura(uint32 spellId, const Unit* unit)
{
    if (!spellId || !unit)
        return false;

    for (uint32 effect = EFFECT_INDEX_0; effect <= EFFECT_INDEX_2; effect++)
    {
        Aura* aura = ((Unit*)unit)->GetAura(spellId, (SpellEffectIndex)effect);

        if (IsRealAura(bot, aura, (Unit*)unit))
            return true;
    }

    return false;
}

bool PlayerbotAI::HasAnyAuraOf(Unit* player, ...)
{
    if (!player)
        return false;

    va_list vl;
    va_start(vl, player);

    const char* cur;
    do {
        cur = va_arg(vl, const char*);
        if (cur && HasAura(cur, player)) {
            va_end(vl);
            return true;
        }
    }
    while (cur);

    va_end(vl);
    return false;
}

bool PlayerbotAI::CanCastSpell(string name, Unit* target)
{
    return CanCastSpell(aiObjectContext->GetValue<uint32>("spell id", name)->Get(), target);
}

bool PlayerbotAI::CanCastSpell(uint32 spellid, Unit* target, bool checkHasSpell)
{
    if (!spellid)
        return false;

    if (!target)
        target = bot;

    if (checkHasSpell && !bot->HasSpell(spellid))
        return false;

    if (bot->HasSpellCooldown(spellid))
        return false;

    bool positiveSpell = IsPositiveSpell(spellid);
    if (positiveSpell && bot->IsHostileTo(target))
        return false;

    if (!positiveSpell && bot->IsFriendlyTo(target))
        return false;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellid );
    if (!spellInfo)
        return false;

    if (target->IsImmuneToSpell(spellInfo))
        return false;

    ObjectGuid oldSel = bot->GetSelectionGuid();
    bot->SetSelectionGuid(target->GetObjectGuid());
    Spell *spell = new Spell(bot, spellInfo, false );

    spell->m_targets.setUnitTarget(target);
    spell->m_CastItem = aiObjectContext->GetValue<Item*>("item for spell", spellid)->Get();
    spell->m_targets.setItemTarget(spell->m_CastItem);
    SpellCastResult result = spell->CheckCast(false);
    delete spell;
    bot->SetSelectionGuid(oldSel);

    switch (result)
    {
    case SPELL_FAILED_NOT_INFRONT:
    case SPELL_FAILED_NOT_STANDING:
    case SPELL_FAILED_UNIT_NOT_INFRONT:
    case SPELL_FAILED_SUCCESS:
    case SPELL_FAILED_MOVING:
    case SPELL_FAILED_TRY_AGAIN:
    case SPELL_FAILED_NOT_IDLE:
    case SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW:
    case SPELL_FAILED_SUMMON_PENDING:
    case SPELL_FAILED_BAD_IMPLICIT_TARGETS:
    case SPELL_FAILED_BAD_TARGETS:
    case SPELL_CAST_OK:
    case SPELL_FAILED_ITEM_NOT_FOUND:
        return true;
    default:
        return false;
    }
}


bool PlayerbotAI::CastSpell(string name, Unit* target)
{
    return CastSpell(aiObjectContext->GetValue<uint32>("spell id", name)->Get(), target);
}

bool PlayerbotAI::CastSpell(uint32 spellId, Unit* target)
{
    if (!spellId)
        return false;

    if (!target)
        target = bot;

    Pet* pet = bot->GetPet();
    if (pet && pet->HasSpell(spellId))
    {
        pet->ToggleAutocast(spellId, true);
        TellMaster("My pet will auto-cast this spell");
        return true;
    }

    aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get().Set(spellId, target->GetObjectGuid(), time(0));
    aiObjectContext->GetValue<LastMovement&>("last movement")->Get().Set(NULL);

    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);

    MotionMaster &mm = *bot->GetMotionMaster();
    if (bot->isMoving() && GetSpellCastTime(pSpellInfo, NULL))
    {
        return false;
    }

    bot->clearUnitState( UNIT_STAT_CHASE );
    bot->clearUnitState( UNIT_STAT_FOLLOW );

    ObjectGuid oldSel = bot->GetSelectionGuid();
    bot->SetSelectionGuid(target->GetObjectGuid());

    Spell *spell = new Spell(bot, pSpellInfo, false);

    SpellCastTargets targets;
    targets.setUnitTarget(target);
    WorldObject* faceTo = target;

    if (pSpellInfo->Targets & TARGET_FLAG_ITEM)
    {
        spell->m_CastItem = aiObjectContext->GetValue<Item*>("item for spell", spellId)->Get();
        targets.setItemTarget(spell->m_CastItem);
    }

    if (pSpellInfo->Effect[0] == SPELL_EFFECT_OPEN_LOCK ||
        pSpellInfo->Effect[0] == SPELL_EFFECT_SKINNING)
    {
        LootObject loot = *aiObjectContext->GetValue<LootObject>("loot target");
        if (!loot.IsLootPossible(bot))
            return false;

        GameObject* go = GetGameObject(loot.guid);
        if (go && go->isSpawned())
        {
            WorldPacket* const packetgouse = new WorldPacket(CMSG_GAMEOBJ_REPORT_USE, 8);
            *packetgouse << loot.guid;
            bot->GetSession()->QueuePacket(packetgouse);  // queue the packet to get around race condition
            targets.setGOTarget(go);
            faceTo = go;
        }
        else
        {
            Creature* creature = GetCreature(loot.guid);
            if (creature)
            {
                targets.setUnitTarget(creature);
                faceTo = creature;
            }
        }
    }


    if (!bot->isInFront(faceTo, sPlayerbotAIConfig.spellDistance))
    {
        if (sPlayerbotAIConfig.splineFacing)
            bot->SetFacingTo(bot->GetAngle(faceTo));
        else
            bot->SetOrientation(bot->GetAngle(faceTo));
        return false;
    }

    WaitForSpellCast(spellId);

    spell->prepare(&targets, false);
    bot->SetSelectionGuid(oldSel);

    LastSpellCast& lastSpell = aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get();
    return lastSpell.id == spellId;
}

void PlayerbotAI::WaitForSpellCast(uint32 spellId)
{
    const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);

    float castTime = GetSpellCastTime(pSpellInfo) + sPlayerbotAIConfig.reactDelay;
    if (IsChanneledSpell(pSpellInfo))
    {
        int32 duration = GetSpellDuration(pSpellInfo);
        if (duration > 0)
            castTime += duration;
    }

    castTime = ceil(castTime);

    uint32 globalCooldown = CalculateGlobalCooldown(spellId);
    if (castTime < globalCooldown)
        castTime = globalCooldown;

    SetNextCheckDelay(castTime);
}

void PlayerbotAI::InterruptSpell()
{
    if (bot->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        return;

    LastSpellCast& lastSpell = aiObjectContext->GetValue<LastSpellCast&>("last spell cast")->Get();
    WorldPacket* const packet = new WorldPacket(CMSG_CANCEL_CAST, 5);
    *packet << lastSpell.id;
    *packet << lastSpell.target;
    bot->GetSession()->QueuePacket(packet);

    for (int type = CURRENT_MELEE_SPELL; type < CURRENT_CHANNELED_SPELL; type++)
        bot->InterruptSpell((CurrentSpellTypes)type);

    for (int type = CURRENT_MELEE_SPELL; type < CURRENT_CHANNELED_SPELL; type++)
        bot->GetMover()->InterruptSpell((CurrentSpellTypes)type);

    SpellInterrupted(lastSpell.id);
}


void PlayerbotAI::RemoveAura(string name)
{
    uint32 spellid = aiObjectContext->GetValue<uint32>("spell id", name)->Get();
    if (spellid && HasAura(spellid, bot))
        bot->RemoveAurasDueToSpell(spellid);
}

bool PlayerbotAI::IsInterruptableSpellCasting(Unit* target, string spell)
{
    uint32 spellid = aiObjectContext->GetValue<uint32>("spell id", spell)->Get();
    if (!spellid || !target->IsNonMeleeSpellCasted(true))
        return false;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellid );
    if (!spellInfo)
        return false;

    if (target->IsImmuneToSpell(spellInfo))
        return false;

    for (int32 i = EFFECT_INDEX_0; i <= EFFECT_INDEX_2; i++)
    {
        if ((spellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_INTERRUPT) && spellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE)
            return true;

        if ((spellInfo->Effect[i] == SPELL_EFFECT_CANCEL_AURA || spellInfo->Effect[i] == SPELL_EFFECT_INTERRUPT_CAST) &&
                !target->IsImmuneToSpellEffect(spellInfo, (SpellEffectIndex)i))
            return true;
    }

    return false;
}

bool PlayerbotAI::HasAuraToDispel(Unit* target, uint32 dispelType)
{
    for (uint32 type = SPELL_AURA_NONE; type < TOTAL_AURAS; ++type)
    {
        Unit::AuraList auras = target->GetAurasByType((AuraType)type);
        for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
        {
            Aura* aura = *itr;
            const SpellEntry* entry = aura->GetSpellProto();
            uint32 spellId = entry->Id;

            bool isPositiveSpell = IsPositiveSpell(spellId);
            if (isPositiveSpell && bot->IsFriendlyTo(target))
                continue;

            if (!isPositiveSpell && bot->IsHostileTo(target))
                continue;

            if (canDispel(entry, dispelType))
                return true;
        }
    }
    return false;
}

#ifndef WIN32
inline int strcmpi(const char* s1, const char* s2)
{
    for (; *s1 && *s2 && (toupper(*s1) == toupper(*s2)); ++s1, ++s2);
    return *s1 - *s2;
}
#endif

bool PlayerbotAI::canDispel(const SpellEntry* entry, uint32 dispelType)
{
    if (entry->Dispel != dispelType)
        return false;

    return !entry->SpellName[0] ||
        (strcmpi((const char*)entry->SpellName[0], "demon skin") &&
        strcmpi((const char*)entry->SpellName[0], "mage armor") &&
        strcmpi((const char*)entry->SpellName[0], "frost armor") &&
        strcmpi((const char*)entry->SpellName[0], "wavering will") &&
        strcmpi((const char*)entry->SpellName[0], "chilled") &&
        strcmpi((const char*)entry->SpellName[0], "ice armor"));
}

inline bool IsAlliance(uint8 race)
{
    return race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF ||
            race == RACE_GNOME || race == RACE_DRAENEI;
}

bool PlayerbotAI::IsOpposing(Player* player)
{
    if (IsAlliance(player->getRace()))
        return !IsAlliance(bot->getRace());

    return IsAlliance(bot->getRace());
}

void PlayerbotAI::RemoveShapeshift()
{
    RemoveAura("bear form");
    RemoveAura("dire bear form");
    RemoveAura("moonkin form");
    RemoveAura("travel form");
    RemoveAura("cat form");
    RemoveAura("flight form");
    RemoveAura("swift flight form");
    RemoveAura("aquatic form");
    RemoveAura("ghost wolf");
    RemoveAura("tree of life");
}
