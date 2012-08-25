#pragma once

#include "../../shared/Config/Config.h"

class Player;
class PlayerbotMgr;
class ChatHandler;

class PlayerbotAIConfig
{
public:
    PlayerbotAIConfig();

public:
    bool Initialize();
    bool IsInRandomAccountList(uint32 id);

    bool enabled;
    bool allowGuildBots;
    uint32 globalCoolDown, reactDelay, teleportDelay;
    float sightDistance, spellDistance, reactDistance, grindDistance, lootDistance, fleeDistance, tooCloseDistance, meleeDistance, followDistance;
    uint32 criticalHealth, lowHealth, mediumHealth, almostFullHealth;
    uint32 lowMana, mediumMana;

    bool randomBotAutologin;
    std::list<uint32> randomBotAccounts;
    std::vector<uint32> randomBotMaps;
    uint32 randomBotTeleportDistance;
    uint32 randomGearQuality;
    uint32 minRandomBots, maxRandomBots;
    uint32 randomBotUpdateInterval, randomBotCountChangeInterval;
    uint32 minRandomBotInWorldTime, maxRandomBotInWorldTime;
    uint32 minRandomBotRandomizeTime, maxRandomRandomizeTime;
    uint32 minRandomBotPvpTime, maxRandomBotPvpTime;
    uint32 minRandomBotsPerInterval, maxRandomBotsPerInterval;
    uint32 randomBotEnemyPercent;

    uint32 iterationsPerTick;
    bool splineFacing;

    std::string GetValue(std::string name);
    void SetValue(std::string name, std::string value);

private:
    Config config;
};

#define sPlayerbotAIConfig MaNGOS::Singleton<PlayerbotAIConfig>::Instance()
