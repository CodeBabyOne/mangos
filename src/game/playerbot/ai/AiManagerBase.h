#pragma once

class Player;
class ChatHandler;

using namespace std;

class PlayerbotChatHandler: protected ChatHandler
{
public:
	explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer) {}
	bool revive(const Player& botPlayer) { return HandleReviveCommand((char*)botPlayer.GetName()); }
	bool teleport(const Player& botPlayer) { return HandleNamegoCommand((char*)botPlayer.GetName()); }
	void sysmessage(const char *str) { SendSysMessage(str); }
	bool dropQuest(const char *str) { return HandleQuestRemoveCommand((char*)str); }
    uint32 extractQuestId(const char *str);
};

class PlayerbotAI;

namespace ai 
{
	class AiManagerRegistry;

	class AiManagerBase
	{
	public:
		AiManagerBase(PlayerbotAI* ai, AiManagerRegistry* aiRegistry);

	public:
        virtual void Update() {}
		virtual void HandleCommand(const string& text, Player& fromPlayer) {}
		virtual void HandleBotOutgoingPacket(const WorldPacket& packet) {}
        virtual void HandleMasterIncomingPacket(const WorldPacket& packet) {}
        virtual void Query(const string& text) {}
		uint64 extractGuid(WorldPacket& packet);

	protected:
		PlayerbotAI* ai;
		Player* bot;
		AiManagerRegistry* aiRegistry;
	};

};
