#pragma once;

#include "Category.h"
#include "ItemBag.h"
#include "../playerbot/PlayerbotAIBase.h"
#include "../AuctionHouseMgr.h"
#include "../ObjectGuid.h"
#include "../WorldSession.h"

#define MAX_AUCTIONS 7
#define AHBOT_WON_EXPIRE 0
#define AHBOT_WON_PLAYER 1
#define AHBOT_WON_SELF 2
#define AHBOT_WON_BID 3
#define AHBOT_WON_DELAY 4

namespace ahbot
{
    using namespace std;

    class AhBot
    {
    public:
        AhBot() : nextAICheckTime(0), player(NULL) {}
        virtual ~AhBot();

    public:
        ObjectGuid GetAHBplayerGUID();
        void Init();
        void Update();
        void ForceUpdate();
        void HandleCommand(string command);
        void Won(AuctionEntry* entry) { AddToHistory(entry); }
        void Expired(AuctionEntry* entry) {}

        double GetCategoryMultiplier(string category)
        {
            return categoryMultipliers[category];
        }

        int32 GetSellPrice(const ItemPrototype* proto);

    private:
        void Update(int auction, ItemBag* inAuction);
        void Update(int auction, Category* category, ItemBag* inAuctionItems);
        void Answer(int auction, Category* category, ItemBag* inAuctionItems);
        void AddAuctions(int auction, Category* category, ItemBag* inAuctionItems);
        void AddAuction(int auction, Category* category, const ItemPrototype* proto);
        void Expire(int auction);
        void AddToHistory(AuctionEntry* entry, uint32 won = 0);
        void CleanupHistory();
        uint32 GetAvailableMoney(uint32 auctionHouse);
        void CheckCategoryMultipliers();
        void updateMarketPrice(uint32 itemId, double price, uint32 auctionHouse);
        bool IsBotAuction(uint32 bidder);
        uint32 GetRandomBidder(uint32 auctionHouse);
        void LoadRandomBots();
        uint32 GetAnswerCount(uint32 itemId, uint32 auctionHouse, uint32 withinTime);
        vector<AuctionEntry*> LoadAuctions(const AuctionHouseObject::AuctionEntryMap& auctionEntryMap, Category*& category,
                int& auction);
        void FindMinPrice(const AuctionHouseObject::AuctionEntryMap& auctionEntryMap, AuctionEntry*& entry, Item*& item, uint32* minBid,
                uint32* minBuyout);
        uint32 GetBuyTime(uint32 itemId, uint32 auctionHouse, string category);

    public:
        static uint32 auctionIds[MAX_AUCTIONS];
        static map<uint32, uint32> factions;

    private:
        AvailableItemsBag availableItems;
        WorldSession* session;
        Player* player;
        time_t nextAICheckTime;
        map<string, double> categoryMultipliers;
        map<string, uint64> categoryMultiplierExpireTimes;
        map<uint32, vector<uint32> > bidders;
        set<uint32> allBidders;
    };
};

#define auctionbot MaNGOS::Singleton<ahbot::AhBot>::Instance()
