#pragma once

#include "PlayerbotMgr.h"
#include "PlayerbotAIBase.h"
#include "strategy/AiObjectContext.h"
#include "strategy/Engine.h"
#include "strategy/ExternalEventHelper.h"
#include "ChatFilter.h"

class Player;
class PlayerbotMgr;
class ChatHandler;

using namespace std;
using namespace ai;

class PlayerbotChatHandler: protected ChatHandler
{
public:
    explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
    bool revive(const Player& botPlayer) { return HandleReviveCommand((char*)botPlayer.GetName()); }
    bool teleport(const Player& botPlayer) { return HandleNamegoCommand((char*)botPlayer.GetName()); }
    void sysmessage(string str) { SendSysMessage(str.c_str()); }
    bool dropQuest(string str) { return HandleQuestRemoveCommand((char*)str.c_str()); }
    uint32 extractQuestId(string str);
    uint32 extractSpellId(string str) 
    { 
        char* source = (char*)str.c_str();
        return ExtractSpellIdFromLink(&source); 
    }
};

namespace ai
{
	class MinValueCalculator {
	public:
		MinValueCalculator(float def = 0.0f) {
			param = NULL;
			minValue = def;
		}

	public:
		void probe(float value, void* p) {
			if (!param || minValue >= value) {
				minValue = value;
				param = p;
			}
		}

	public:
		void* param;
		float minValue;
	};
};

enum BotState
{
    BOT_STATE_COMBAT = 0,
    BOT_STATE_NON_COMBAT = 1,
    BOT_STATE_DEAD = 2
};

#define BOT_STATE_MAX 3

class PacketHandlingHelper
{
public:
    void AddHandler(uint16 opcode, string handler);
    void Handle(ExternalEventHelper &helper);
    void AddPacket(const WorldPacket& packet);

private:
    map<uint16, string> handlers;
    stack<WorldPacket> queue;
};

class PlayerbotAI : public PlayerbotAIBase
{
public:
	PlayerbotAI();
	PlayerbotAI(PlayerbotMgr* mgr, Player* bot, NamedObjectContext<UntypedValue>* sharedValues);
	virtual ~PlayerbotAI();

public:
	virtual void UpdateAI(uint32 elapsed);
	virtual void UpdateAIInternal(uint32 elapsed);
    void HandleCommand(uint32 type, const string& text, Player& fromPlayer);
	void HandleBotOutgoingPacket(const WorldPacket& packet);
    void HandleMasterIncomingPacket(const WorldPacket& packet);
    void HandleMasterOutgoingPacket(const WorldPacket& packet);
	void HandleTeleportAck();
    void ChangeActiveEngineIfNecessary();
    void ChangeEngine(BotState type);
    void DoNextAction();
    void DoSpecificAction(string name);
    void ChangeStrategy(string name, BotState type);
    bool ContainsStrategy(StrategyType type);
    bool HasStrategy(string name, BotState type);
    void ResetStrategies();
    void ReInitCurrentEngine();
    void Reset();
    void OnBotLogin();
    void DoPvpAttack();
    void RandomTeleport();
    bool IsTank(Player* player);
    bool IsHeal(Player* player);
    Creature* GetCreature(ObjectGuid guid);
    Unit* GetUnit(ObjectGuid guid);
    GameObject* GetGameObject(ObjectGuid guid);
    void TellMaster(ostringstream &stream) { TellMaster(stream.str()); }
    void TellMaster(string text);
    void TellMaster(LogLevel level, string text);
    void SpellInterrupted(uint32 spellid);
    int32 CalculateGlobalCooldown(uint32 spellid);
    void InterruptSpell();
    void RemoveAura(string name);
    void RemoveShapeshift();

    virtual bool CanCastSpell(string name, Unit* target);
    virtual bool CastSpell(string name, Unit* target);
    virtual bool HasAura(string spellName, Unit* player);
    virtual bool HasAnyAuraOf(Unit* player, ...);

    virtual bool IsInterruptableSpellCasting(Unit* player, string spell);
    virtual bool HasAuraToDispel(Unit* player, uint32 dispelType);
    bool CanCastSpell(uint32 spellid, Unit* target);

    bool HasAura(uint32 spellId, const Unit* player);
    bool CastSpell(uint32 spellId, Unit* target);
    bool canDispel(const SpellEntry* entry, uint32 dispelType);

public:
	Player* GetBot() { return bot; }
    Player* GetMaster() { return mgr ? mgr->GetMaster() : NULL; }
    AiObjectContext* GetAiObjectContext() { return aiObjectContext; }
    ChatHelper* GetChatHelper() { return &chatHelper; }
    bool IsOpposing(Player* player);

protected:
	Player* bot;
	PlayerbotMgr* mgr;
    AiObjectContext* aiObjectContext;
    Engine* currentEngine;
    Engine* engines[BOT_STATE_MAX];
    ChatHelper chatHelper;
    stack<string> chatCommands;
    PacketHandlingHelper botOutgoingPacketHandlers;
    PacketHandlingHelper masterIncomingPacketHandlers;
    PacketHandlingHelper masterOutgoingPacketHandlers;
    CompositeChatFilter chatFilter;
};

