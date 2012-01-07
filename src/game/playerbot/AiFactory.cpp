#include "../pchdef.h"
#include "playerbot.h"
#include "AiFactory.h"
#include "strategy/Engine.h"

#include "strategy/priest/PriestAiObjectContext.h"
#include "strategy/mage/MageAiObjectContext.h"
#include "strategy/warlock/WarlockAiObjectContext.h"
#include "strategy/warrior/WarriorAiObjectContext.h"
#include "strategy/shaman/ShamanAiObjectContext.h"
#include "strategy/paladin/PaladinAiObjectContext.h"
#include "strategy/druid/DruidAiObjectContext.h"
#include "strategy/hunter/HunterAiObjectContext.h"
#include "strategy/rogue/RogueAiObjectContext.h"
#include "../Player.h"


AiObjectContext* AiFactory::createAiObjectContext(Player* player, PlayerbotAI* ai)
{
    switch (player->getClass())
    {
    case CLASS_PRIEST:
        return new PriestAiObjectContext(ai);
        break;
    case CLASS_MAGE:
        return new MageAiObjectContext(ai);
        break;
    case CLASS_WARLOCK:
        return new WarlockAiObjectContext(ai);
        break;
    case CLASS_WARRIOR:
        return new WarriorAiObjectContext(ai);
        break;
    case CLASS_SHAMAN:
        return new ShamanAiObjectContext(ai);
        break;
    case CLASS_PALADIN:
        return new PaladinAiObjectContext(ai);
        break;
    case CLASS_DRUID:
        return new DruidAiObjectContext(ai);
        break;
    case CLASS_HUNTER:
        return new HunterAiObjectContext(ai);
        break;
    case CLASS_ROGUE:
        return new RogueAiObjectContext(ai);
        break;
    }
    return NULL;
}

int AiFactory::GetPlayerSpecTab(Player* player)
{
    map<uint32,int32> tabs;
    for(uint32 i = 0;i < uint32(3);i++)
        tabs[i] = 0;

    uint8 activeSpec = player->GetActiveSpec();
    PlayerTalentMap talents = player->GetTalentMap(activeSpec);
    for(PlayerTalentMap::iterator iter = talents.begin();iter != talents.end();++iter){
        if(iter->second.state == PLAYERSPELL_REMOVED)
            continue;

        const TalentEntry *talentInfo = iter->second.talentEntry;
        if(!talentInfo)
            continue;

        const TalentTabEntry *talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if(!talentTabInfo)
            continue;

        if((player->getClassMask() & talentTabInfo->ClassMask) == 0)
            continue;

        tabs[talentTabInfo->tabpage]++;
    }
    int tab = -1, max = 0;
    for(uint32 i = 0;i < uint32(3);i++){
        if(tab == -1 || max < tabs[i]){
            tab = i;
            max = tabs[i];
        }
    }

    return tab;
}

void AiFactory::AddDefaultCombatStrategies(Player* player, Engine* engine)
{
    int tab = GetPlayerSpecTab(player);
    
    engine->addStrategies("flee", "attack weak", "racials", "chat", "default", "aoe", "potions", "cast time", "conserve mana", NULL);

    switch (player->getClass()){
        case CLASS_PRIEST:
            engine->addStrategy(tab == 2 ? "dps" : "heal");
            break;
        case CLASS_MAGE:
            if (tab == 0)
                engine->addStrategies("arcane", NULL);
            else if (tab == 1)
                engine->addStrategies("fire", "fire aoe", NULL);
            else
                engine->addStrategies("frost", "frost aoe", NULL);
            break;
        case CLASS_WARRIOR:
            engine->addStrategy(tab == 2 ? "tank" : "dps");
            break;
        case CLASS_SHAMAN:
            if (tab == 0)
                engine->addStrategies("caster", "caster aoe", "bmana", NULL);
            else if (tab == 2)
                engine->addStrategies("heal", "bmana", NULL);
            else
                engine->addStrategies("dps", "melee aoe", "bdps", NULL);
            break;
        case CLASS_PALADIN:
            if (tab == 1)
                engine->addStrategies("tank", "tank aoe", "barmor", NULL);
            else
                engine->addStrategies("dps", "bdps", NULL);
            break;
        case CLASS_DRUID:
            if (tab == 0)
                engine->addStrategies("caster", "caster aoe", NULL);
            else if (tab == 2)
                engine->addStrategies("heal", NULL);
            else
                engine->addStrategies("bear", "tank aoe", NULL);
            break;
        case CLASS_HUNTER:
            engine->addStrategies("dps", "bdps", NULL);
        case CLASS_ROGUE:
        case CLASS_WARLOCK:
            if (tab == 1)
                engine->addStrategy("tank");
            else
                engine->addStrategy("dps");
            break;
    }
}

Engine* AiFactory::createCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
	Engine* engine = new Engine(facade, AiObjectContext);
    AddDefaultCombatStrategies(player, engine);
    if (facade->IsOpposing(facade->GetMaster()))
        engine->addStrategies("grind", "dps", NULL);
	return engine;
}

void AiFactory::AddDefaultNonCombatStrategies(Player* player, Engine* nonCombatEngine)
{
    int tab = GetPlayerSpecTab(player);

    switch (player->getClass()){
        case CLASS_PALADIN:
            nonCombatEngine->addStrategy("bspeed");
            break;
        case CLASS_HUNTER:
            nonCombatEngine->addStrategy("bspeed");
            break;
        case CLASS_MAGE:
            if (tab == 1)
                nonCombatEngine->addStrategy("bdps");
            else
                nonCombatEngine->addStrategy("bmana");
            break;
    }
    nonCombatEngine->addStrategies("nc", "attack weak", "food", "stay", "chat",
            "default", "quest", "loot", "gather", "duel", "emote", NULL);
}

Engine* AiFactory::createNonCombatEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
	Engine* nonCombatEngine = new Engine(facade, AiObjectContext);

    AddDefaultNonCombatStrategies(player, nonCombatEngine);
    if (facade->IsOpposing(facade->GetMaster()))
        nonCombatEngine->addStrategy("grind");
	return nonCombatEngine;
}

void AiFactory::AddDefaultDeadStrategies(Engine* deadEngine)
{
    deadEngine->addStrategies("dead", "stay", "chat", "default", "follow master", NULL);
}

Engine* AiFactory::createDeadEngine(Player* player, PlayerbotAI* const facade, AiObjectContext* AiObjectContext) {
    Engine* deadEngine = new Engine(facade, AiObjectContext);
    AddDefaultDeadStrategies(deadEngine);
    return deadEngine;
}
