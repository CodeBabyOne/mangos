#include "Config/Config.h"
#include "../pchdef.h"
#include "playerbot.h"
#include "strategy/values/SharedValueContext.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "../AccountMgr.h"
#include "../ObjectMgr.h"
#include "../../shared/Database/DatabaseEnv.h"
#include "PlayerbotAI.h"
#include "../Player.h"

RandomPlayerbotMgr::RandomPlayerbotMgr(Player* const master) : PlayerbotAIBase(),  master(master)
{
    account = sObjectMgr.GetPlayerAccountIdByGUID(master->GetObjectGuid());
}

RandomPlayerbotMgr::~RandomPlayerbotMgr()
{
}

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed)
{
    SetNextCheckDelay(sPlayerbotAIConfig.randomBotUpdateInterval * 1000);

    if (!sPlayerbotAIConfig.randomBotAutologin)
        return;

    sLog.outBasic("Processing random bots for account %d...", account);

    int maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (!maxAllowedBotCount)
    {
        maxAllowedBotCount = urand(sPlayerbotAIConfig.minRandomBots, sPlayerbotAIConfig.maxRandomBots);
        SetEventValue(0, "bot_count", maxAllowedBotCount, sPlayerbotAIConfig.randomBotCountChangeInterval);
    }

    list<uint32> bots = GetBots();
    int botCount = bots.size();
    int maxEnemyBots = maxAllowedBotCount * sPlayerbotAIConfig.randomBotEnemyPercent / 100;
    while (botCount++ < maxEnemyBots && botCount < maxAllowedBotCount)
    {
        uint32 bot = AddRandomBot(true);
        if (bot)
            bots.push_back(bot);
        else
            break;
    }
    while (botCount++ < maxAllowedBotCount)
    {
        uint32 bot = AddRandomBot(false);
        if (bot)
            bots.push_back(bot);
        else
            break;
    }

    int botProcessed = 0;
    int randomBotsPerInterval = urand(sPlayerbotAIConfig.minRandomBotsPerInterval, sPlayerbotAIConfig.maxRandomBotsPerInterval);
    for (list<uint32>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        uint32 bot = *i;
        if (ProcessBot(bot))
            botProcessed++;

        if (botProcessed >= randomBotsPerInterval)
            break;
    }

    sLog.outString("Random bot processing finished. Next check in %d seconds", sPlayerbotAIConfig.randomBotUpdateInterval);
}

uint32 RandomPlayerbotMgr::AddRandomBot(bool opposing)
{
    vector<uint32> bots = GetFreeBots(opposing);
    if (bots.size() == 0)
        return 0;

    int index = urand(0, bots.size() - 1);
    uint32 bot = bots[index];
    SetEventValue(bot, "add", 1, urand(sPlayerbotAIConfig.minRandomBotInWorldTime, sPlayerbotAIConfig.maxRandomBotInWorldTime));
    SetEventValue(bot, "pvp", 1, urand(sPlayerbotAIConfig.minRandomBotPvpTime, sPlayerbotAIConfig.maxRandomBotPvpTime));
    uint32 randomTime = 30 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3);
    ScheduleRandomize(bot, randomTime);
    sLog.outDetail("Bot %d added for account %d", bot, account);
    return bot;
}

void RandomPlayerbotMgr::ScheduleRandomize(uint32 bot, uint32 time)
{
    SetEventValue(bot, "randomize", 1, time);
    SetEventValue(bot, "logout", 1, time + 30 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3));
}

bool RandomPlayerbotMgr::ProcessBot(uint32 bot)
{
    PlayerbotMgr* mgr = master->GetPlayerbotMgr();

    uint32 isValid = GetEventValue(bot, "add");
    if (!isValid)
    {
		Player* player = mgr->GetPlayerBot(bot);
		if (!player || !player->GetGroup())
		{
			sLog.outBasic("Bot %d expired for account %d", bot, account);
			SetEventValue(bot, "add", 0, 0);
		}
        return true;
    }

    if (!mgr->GetPlayerBot(bot))
    {
        sLog.outBasic("Bot %d logged in for account %d", bot, account);
        mgr->AddPlayerBot(bot, master->GetSession());
        if (!GetEventValue(bot, "online"))
        {
            SetEventValue(bot, "pvp", 1, urand(sPlayerbotAIConfig.minRandomBotPvpTime, sPlayerbotAIConfig.maxRandomBotPvpTime));
            SetEventValue(bot, "online", 1, sPlayerbotAIConfig.minRandomBotInWorldTime);
        }
        return true;
    }

    Player* player = mgr->GetPlayerBot(bot);
    if (!player)
        return false;

    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (!ai)
        return false;

    if (player->GetGroup())
    {
        sLog.outBasic("Skipping bot %d for account %d as it is in group", bot, account);
        return false;
    }

    uint32 randomize = GetEventValue(bot, "randomize");
    if (!randomize)
    {
        sLog.outBasic("Randomizing bot %d for account %d", bot, account);
        Randomize(player);
        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomRandomizeTime);
        ScheduleRandomize(bot, randomTime);
        return true;
    }

    uint32 logout = GetEventValue(bot, "logout");
    if (!logout)
    {
        sLog.outBasic("Logging out bot %d for account %d", bot, account);
        master->GetPlayerbotMgr()->LogoutPlayerBot(bot);
        SetEventValue(bot, "logout", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
        return true;
    }

    if (player->isDead())
    {
        sLog.outBasic("Random teleporting dead bot %d for account %d", bot, account);
        RandomTeleport(player, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());
        return true;
    }

    if (ai->IsOpposing(master) &&
            !master->GetInstanceId() && master->IsAllowedDamageInArea(player) && master->IsPvP() &&
            !master->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) && !master->isInCombat() && player->isAlive())
    {
        uint32 pvp = GetEventValue(bot, "pvp");
        if (!pvp)
        {
            sLog.outBasic("Starting PVP attack with bot %d for account %d", bot, account);
            SetEventValue(bot, "pvp", 1, urand(sPlayerbotAIConfig.minRandomBotPvpTime, sPlayerbotAIConfig.maxRandomBotPvpTime));
            DoPvpAttack(player);
            return true;
        }
    }

    return false;
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot, vector<WorldLocation> &locs)
{
    if (bot->IsBeingTeleported())
        return;

    if (locs.empty())
    {
        sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
        return;
    }

    for (int attemtps = 0; attemtps < 10; ++attemtps)
    {
        int index = urand(0, locs.size() - 1);
        WorldLocation loc = locs[index];
        float x = loc.coord_x + urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2;
        float y = loc.coord_y + urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2;
        float z = loc.coord_z;

        Map* map = sMapMgr.FindMap(loc.mapid);
        if (!map)
            continue;

        const TerrainInfo * terrain = map->GetTerrain();
        if (!terrain->IsOutdoors(x, y, z) ||
                terrain->IsAboveWater(x, y, z) ||
                terrain->IsUnderWater(x, y, z) ||
                terrain->IsInWater(x, y, z))
            continue;

        z = 0.05f + map->GetTerrain()->GetHeightStatic(x, y, 0.05f + z, true, MAX_HEIGHT);
        bot->TeleportTo(loc.mapid, x, y, z, 0);
        return;
    }

    sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
}

void RandomPlayerbotMgr::RandomTeleportForLevel(Player* bot)
{
    vector<WorldLocation> locs;
    QueryResult* results = WorldDatabase.PQuery("select map, position_x, position_y, position_z "
        "from (select map, position_x, position_y, position_z, avg(t.maxlevel), avg(t.minlevel), "
        "(avg(t.maxlevel) + avg(t.minlevel)) / 2 - %u delta "
        "from creature c inner join creature_template t on c.id = t.entry group by t.entry) q "
        "where delta >= 0 and delta <= 1 and map in (%s)",
        bot->getLevel(), sPlayerbotAIConfig.randomBotMapsAsString.c_str());
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 mapId = fields[0].GetUInt32();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            WorldLocation loc(mapId, x, y, z, 0);
            locs.push_back(loc);
        } while (results->NextRow());
        delete results;
    }

    RandomTeleport(bot, locs);
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot, uint32 mapId, float teleX, float teleY, float teleZ)
{
    Refresh(bot);

    vector<WorldLocation> locs;
    QueryResult* results = WorldDatabase.PQuery("select position_x, position_y, position_z from creature where map = '%u' and abs(position_x - '%f') < '%u' and abs(position_y - '%f') < '%u'",
            mapId, teleX, sPlayerbotAIConfig.randomBotTeleportDistance / 2, teleY, sPlayerbotAIConfig.randomBotTeleportDistance / 2);
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            float x = fields[0].GetFloat();
            float y = fields[1].GetFloat();
            float z = fields[2].GetFloat();
            WorldLocation loc(mapId, x, y, z, 0);
            locs.push_back(loc);
        } while (results->NextRow());
        delete results;
    }

    RandomTeleport(bot, locs);
}

void RandomPlayerbotMgr::Randomize(Player* bot)
{
    if (bot->getLevel() == 1)
        RandomizeFirst(bot);
    else
        IncreaseLevel(bot);
}

void RandomPlayerbotMgr::IncreaseLevel(Player* bot)
{
    uint32 maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
    uint32 level = min(bot->getLevel() + 1, maxLevel);
    PlayerbotFactory factory(bot, level);
    factory.Randomize(true);
    RandomTeleportForLevel(bot);
}

void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{
    uint32 maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
    for (int attempt = 0; attempt < 100; ++attempt)
    {
        int index = urand(0, sPlayerbotAIConfig.randomBotMaps.size() - 1);
        uint32 mapId = sPlayerbotAIConfig.randomBotMaps[index];

        vector<GameTele const*> locs;
        GameTeleMap const & teleMap = sObjectMgr.GetGameTeleMap();
        for(GameTeleMap::const_iterator itr = teleMap.begin(); itr != teleMap.end(); ++itr)
        {
            GameTele const* tele = &itr->second;
            if (tele->mapId == mapId)
                locs.push_back(tele);
        }

        index = urand(0, locs.size() - 1);
        GameTele const* tele = locs[index];
        uint32 level = GetZoneLevel(tele->mapId, tele->position_x, tele->position_y, tele->position_z);
        if (level > maxLevel + 5)
            continue;

        level = min(level, maxLevel);

        if (urand(0, 100) < 100 * sPlayerbotAIConfig.randomBotMaxLevelChance)
            level = maxLevel;

        PlayerbotFactory factory(bot, level);
        factory.Randomize(false);
        RandomTeleport(bot, tele->mapId, tele->position_x, tele->position_y, tele->position_z);
        break;
    }
}

uint32 RandomPlayerbotMgr::GetZoneLevel(uint32 mapId, float teleX, float teleY, float teleZ)
{
	uint32 level;
    QueryResult *results = WorldDatabase.PQuery("select avg(t.minlevel) minlevel, avg(t.maxlevel) maxlevel from creature c "
            "inner join creature_template t on c.id = t.entry "
            "where map = '%u' and minlevel > 1 and abs(position_x - '%f') < '%u' and abs(position_y - '%f') < '%u'",
            mapId, teleX, sPlayerbotAIConfig.randomBotTeleportDistance / 2, teleY, sPlayerbotAIConfig.randomBotTeleportDistance / 2);

    if (results)
    {
        Field* fields = results->Fetch();
        uint32 minLevel = fields[0].GetUInt32();
        uint32 maxLevel = fields[1].GetUInt32();
        level = urand(minLevel, maxLevel);
        if (level < 10)
            level = urand(master->getLevel() - 5, master->getLevel());
        if (level > 80)
            level = 80;
        delete results;
    }
    else
    {
        level = urand(master->getLevel() - 5, master->getLevel());
    }

    return level;
}

void RandomPlayerbotMgr::DoPvpAttack(Player* bot)
{
    if (master->IsBeingTeleported())
        return;

	Player* master = bot->GetPlayerbotAI()->GetMaster();
    uint32 level = master->getLevel();

    if (abs((int)bot->getLevel() - (int)level) > 4)
        return;

    WorldLocation loc;
    master->GetPosition(loc);
    float followAngle = frand(0, 2 * M_PI);
    float distance = sPlayerbotAIConfig.grindDistance - frand(sPlayerbotAIConfig.grindDistance / 10, sPlayerbotAIConfig.grindDistance / 5);
    for (float angle = followAngle - M_PI; angle <= followAngle + M_PI; angle += M_PI / 4)
    {
        float x = loc.coord_x + cos(angle) * distance;
        float y = loc.coord_y + sin(angle) * distance;
        float z = loc.coord_z + 10;
        master->UpdateAllowedPositionZ(x, y, z);
        if (master->IsWithinLOS(x, y, z))
        {
            Refresh(bot);
            bot->TeleportTo(master->GetMapId(), x, y, z + 0.05f, 0);
            break;
        }
    }
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
    bot->GetPlayerbotAI()->Reset();

    if (bot->isDead())
    {
        PlayerbotChatHandler ch(bot->GetPlayerbotAI()->GetMaster());
        ch.revive(*bot);
    }

    bot->DurabilityRepairAll(false, 1.0f, false);
    bot->SetHealthPercent(100);
    bot->SetPvP(true);

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));
}


bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
    return IsRandomBot(bot->GetObjectGuid());
}

bool RandomPlayerbotMgr::IsRandomBot(uint32 bot)
{
    return GetEventValue(bot, "add");
}

list<uint32> RandomPlayerbotMgr::GetBots()
{
    list<uint32> bots;

    QueryResult* results = CharacterDatabase.PQuery(
            "select `bot` from ai_playerbot_random_bots where owner = '%u' and event = 'add'", account);

    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            bots.push_back(bot);
        } while (results->NextRow());
        delete results;
    }

    return bots;
}

vector<uint32> RandomPlayerbotMgr::GetFreeBots(bool opposing)
{
    uint8 mastersRace = master->getRace();
    set<uint32> bots;

    QueryResult* results = CharacterDatabase.PQuery(
            "select `bot` from ai_playerbot_random_bots where event = 'add'");

    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            bots.insert(bot);
        } while (results->NextRow());
        delete results;
    }

    vector<uint32> guids;
    for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); i++)
    {
        uint32 accountId = *i;
        if (!sAccountMgr.GetCharactersCount(accountId))
            continue;

        QueryResult *result = CharacterDatabase.PQuery("SELECT guid, race FROM characters WHERE account = '%u'", accountId);
        if (!result)
            continue;

        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint32 race = fields[1].GetUInt32();
            if ((opposing && PlayerbotAI::IsOpposing(race, mastersRace) ||
                    (!opposing && !PlayerbotAI::IsOpposing(race, mastersRace)) &&
                    bots.find(guid) == bots.end()))
                guids.push_back(guid);
        } while (result->NextRow());
        delete result;
    }


    return guids;
}

uint32 RandomPlayerbotMgr::GetEventValue(uint32 bot, string event)
{
    uint32 value = 0;

    QueryResult* results = CharacterDatabase.PQuery(
            "select `value`, `time`, validIn from ai_playerbot_random_bots where owner = '%u' and bot = '%u' and event = '%s'",
            account, bot, event.c_str());

    if (results)
    {
        Field* fields = results->Fetch();
        value = fields[0].GetUInt32();
        uint32 lastChangeTime = fields[1].GetUInt32();
        uint32 validIn = fields[2].GetUInt32();
        if ((time(0) - lastChangeTime) >= validIn)
            value = 0;
        delete results;
    }

    return value;
}

uint32 RandomPlayerbotMgr::SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn)
{
    CharacterDatabase.PExecute("delete from ai_playerbot_random_bots where owner = '%u' and bot = '%u' and event = '%s'",
            account, bot, event.c_str());
    if (value)
    {
        CharacterDatabase.PExecute(
                "insert into ai_playerbot_random_bots (owner, bot, `time`, validIn, event, `value`) values ('%u', '%u', '%u', '%u', '%s', '%u')",
                account, bot, (uint32)time(0), validIn, event.c_str(), value);
    }

    return value;
}

bool ChatHandler::HandlePlayerbotConsoleCommand(char* args)
{
    if(sConfig.GetBoolDefault("PlayerbotAI.DisableBots", false))
    {
        sLog.outError("Playerbot system is currently disabled!");
        return false;
    }

    if (!args || !*args)
    {
        sLog.outError("Usage: rndbot reset");
        return false;
    }

    string cmd = args;

    if (cmd == "reset")
    {
        CharacterDatabase.PExecute("delete from ai_playerbot_random_bots");
        sLog.outBasic("Random bots were reset for all players");
        return true;
    }

    return false;
}
