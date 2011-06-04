#pragma once
#include "LootManager.h"

using namespace std;

namespace ai 
{
	class AiManagerRegistry;
    class AiManagerBase;

    class IterateItemsVisitor 
    {
    public:
        IterateItemsVisitor() {}

        virtual bool Visit(Item* item) = 0;
    };

    class FindItemVisitor : public IterateItemsVisitor {
    public:
        FindItemVisitor() : IterateItemsVisitor(), result(NULL) {}

        virtual bool Visit(Item* item)
        {
            if (!Accept(item->GetProto()))
                return true;

            result = item;
            return false;
        }

        Item* GetResult() { return result; }

    protected:
        virtual bool Accept(const ItemPrototype* proto) = 0;

    private:
        Item* result;
    };

    enum IterateItemsMask
    {
        ITERATE_ITEMS_IN_BAGS = 1,
        ITERATE_ITEMS_IN_EQUIP = 2,
        ITERATE_ALL_ITEMS = 255
    };

	class AiInventoryManager : public AiManagerBase
	{
	public:
		AiInventoryManager(PlayerbotAI* ai, AiManagerRegistry* aiRegistry);
        virtual ~AiInventoryManager();

	public:
		virtual void UseHealingPotion();
        virtual bool HasHealingPotion();
		
        virtual void UseManaPotion();
        virtual bool HasManaPotion();
		
        virtual void UsePanicPotion();
        virtual bool HasPanicPotion();
        
        virtual void UseFood();
        virtual bool HasFood();
        
        virtual void UseDrink();
		virtual bool HasDrink();

		virtual int GetItemCount(const char* name);
		virtual void FindAndUse(const char* item, uint8 delay = 0);
		
        virtual void ClearLoot() { lootManager->ClearLoot(); }
        virtual void AddLoot(ObjectGuid guid) { lootManager->AddLoot(guid); }
        virtual bool CanLoot() { return lootManager->CanLoot(); }
        virtual void DoLoot();
		virtual void AddAllLoot();
		
        virtual void UnequipItem(const char* link);
        virtual void EquipItem(const char* link);
		virtual void UseItem(const char* link);
		virtual void Reward(const char* link);
        
        virtual void Buy(const char* link);
        
        virtual void Sell(string link);
        virtual void Sell(Item* item);
        
        virtual void ListCount(const char* link);
        virtual void ListQuestItems();
		
        virtual void ItemLocalization(std::string& itemName, const uint32 itemID);
		virtual void extractItemIds(const string& text, list<uint32>& itemIds);

        virtual void Trade(const char* text);
        virtual void AcceptTrade();
        virtual void BeginTrade();

	public:
		virtual void HandleCommand(const string& text, Player& fromPlayer);
		virtual void HandleBotOutgoingPacket(const WorldPacket& packet);
        virtual void HandleMasterIncomingPacket(const WorldPacket& packet);
        virtual void Query(const string& text);
        virtual void QueryItemUsage(ItemPrototype const *item);

	private:
        void UseItem(FindItemVisitor* visitor, const uint32 delay = 0);
        bool HasItem(FindItemVisitor* visitor);
		void UseItem(Item& item);
		
        void EquipItem(Item& item);
        void EquipItem(FindItemVisitor* visitor);

        void UnequipItem(Item& item);
        void UnequipItem(FindItemVisitor* visitor);

        void Sell(FindItemVisitor* visitor);

        void QueryItemsUsage(list<uint32> items);
        void QueryItemCount(ItemPrototype const * item);
        void IterateItems(IterateItemsVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        void IterateItemsInBags(IterateItemsVisitor* visitor);
        void IterateItemsInEquip(IterateItemsVisitor* visitor);

        bool TradeItem(const Item& item, int8 slot = -1);
        bool TradeItem(FindItemVisitor *visitor, int8 slot = -1);

		uint32 TextToItemQuality(const char* text);
		void TellItem(ItemPrototype const * proto, int count);
		
	private:
        LootManager *lootManager;
	};

};
