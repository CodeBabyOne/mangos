#pragma once

using namespace std;

namespace ai 
{
    class AiManagerRegistry;
    class AiManagerBase;

	class AiSpellManager : public AiManagerBase
	{
	public:
		AiSpellManager(PlayerbotAI* ai, AiManagerRegistry* aiRegistry);

	public:
		virtual uint32 GetSpellId(const char* args);
		virtual void InterruptSpell();
		virtual void SpellInterrupted(uint32 spellid);
        virtual void FinishSpell();
		virtual void RemoveAura(const char* name);
		virtual bool IsSpellCastUseful(const char* name, Unit* target);

		virtual bool CanCastSpell(const char* name, Unit* target)
		{
			return CanCastSpell(GetSpellId(name), target);
		}

		virtual bool CastSpell(const char* name, Unit* target) 
		{
			return CastSpell(GetSpellId(name), target);
		}

		virtual bool HasAura(const char* spellName, Unit* player) 
		{
			return HasAura(GetSpellId(spellName), player);
		}
		virtual bool HasAnyAuraOf(Unit* player, ...);

		virtual bool IsSpellCasting(Unit* player);
		virtual bool HasAuraToDispel(Unit* player, uint32 dispelType);
        bool CanCastSpell(uint32 spellid, Unit* target);

		uint32 FindSpellId(const char* args);
		bool HasAura(uint32 spellId, const Unit* player);
		bool CastSpell(uint32 spellId, Unit* target);
		bool canDispel(const SpellEntry* entry, uint32 dispelType);

	public:
		uint32 GetLastSpellId() { return lastSpellId; }
		uint64 GetLastSpellTarget() { return lastSpellTarget.GetRawValue(); }
		int32 CalculateGlobalCooldown(uint32 spellid);
        Item* FindItemForSpell(const SpellEntry* const pSpellInfo);
	
	public:
		virtual void HandleCommand(const string& text, Player& fromPlayer);
		virtual void HandleBotOutgoingPacket(const WorldPacket& packet);

	private:
		std::map<std::string, uint32> spellMap;

		uint32 lastSpellId;
		ObjectGuid lastSpellTarget;
		time_t lastCastTime;
	};

};
