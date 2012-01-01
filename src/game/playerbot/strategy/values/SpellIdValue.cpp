#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "SpellIdValue.h"

using namespace ai;

uint32 SpellIdValue::Calculate()
{
    
    

    string namepart = qualifier;
    wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return 0;

    wstrToLower(wnamepart);

    int loc = bot->GetSession()->GetSessionDbcLocale();

    uint32 foundSpellId = 0;
    bool foundMatchUsesNoReagents = false;

    for (PlayerSpellMap::iterator itr = bot->GetSpellMap().begin(); itr != bot->GetSpellMap().end(); ++itr) 
    {
        uint32 spellId = itr->first;

        if (itr->second.state == PLAYERSPELL_REMOVED || itr->second.disabled || IsPassiveSpell(spellId))
            continue;

        const SpellEntry* pSpellInfo = sSpellStore.LookupEntry(spellId);
        if (!pSpellInfo)
            continue;

        if (pSpellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL)
            continue;

        const string name = pSpellInfo->SpellName[loc];
        if (name.empty() || name.length() != wnamepart.length() || !Utf8FitTo(name, wnamepart))
            continue;

        bool usesNoReagents = (pSpellInfo->Reagent[0] <= 0);

        // if we already found a spell
        bool useThisSpell = true;
        if (foundSpellId > 0) {
            if (usesNoReagents && !foundMatchUsesNoReagents) {}
            else if (spellId > foundSpellId) {}
            else
                useThisSpell = false;
        }
        if (useThisSpell) {
            foundSpellId = spellId;
            foundMatchUsesNoReagents = usesNoReagents;
        }
    }

    return foundSpellId;
}
