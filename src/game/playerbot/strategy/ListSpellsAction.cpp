#include "../../pchdef.h"
#include "../playerbot.h"
#include "ListSpellsAction.h"

using namespace ai;

bool ListSpellsAction::Execute(Event event)
{
    Player*  bot = ai->GetAi()->GetBot();
    Player* master = bot->GetPlayerbotAI()->GetMaster();

    int loc = master->GetSession()->GetSessionDbcLocale();

    std::ostringstream posOut;
    std::ostringstream negOut;

    const std::string ignoreList = ",Opening,Closing,Stuck,Remove Insignia,Opening - No Text,Grovel,Duel,Honorless Target,";
    std::string alreadySeenList = ",";

    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr) {
        const uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* const pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        //|| name.find("Teleport") != -1

        std::string comp = ",";
        comp.append(pSpellInfo->SpellName[loc]);
        comp.append(",");

        if (!(ignoreList.find(comp) == std::string::npos && alreadySeenList.find(comp) == std::string::npos))
            continue;

        alreadySeenList += pSpellInfo->SpellName[loc];
        alreadySeenList += ",";

        if (IsPositiveSpell(spellId))
            posOut << " |cffffffff|Hspell:" << spellId << "|h["
            << pSpellInfo->SpellName[loc] << "]|h|r";
        else
            negOut << " |cffffffff|Hspell:" << spellId << "|h["
            << pSpellInfo->SpellName[loc] << "]|h|r";
    }

    ai->GetAi()->TellMaster("here's my non-attack spells:");
    ai->GetAi()->TellMaster(posOut);

    ai->GetAi()->TellMaster("here's my attack spells:");
    ai->GetAi()->TellMaster(negOut);

    return true;
}

