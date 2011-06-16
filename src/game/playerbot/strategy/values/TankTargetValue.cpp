#include "../../../pchdef.h"
#include "../../playerbot.h"
#include "TankTargetValue.h"

using namespace ai;

class FindTargetForTankStrategy : public FindTargetStrategy
{
public:
    FindTargetForTankStrategy(PlayerbotAI* ai) : FindTargetStrategy(ai)
    {
        minThreat = 0;
        minTankCount = 0;
        maxDpsCount = 0;
    }

protected:
    virtual void CheckAttacker(Player* bot, Player* player, ThreatManager* threatManager)
    {
        float threat = threatManager->getThreat(bot);
        Unit* creature = threatManager->getOwner();
        int tankCount, dpsCount;
        GetPlayerCount(bot, creature, &tankCount, &dpsCount);

        if (!result || 
            (bot->GetSelectionGuid() != creature->GetObjectGuid() && minThreat >= threat && 
            (minTankCount >= tankCount || maxDpsCount <= dpsCount)))
        {
            minThreat = threat;
            minTankCount = tankCount;
            maxDpsCount = dpsCount;
            result = creature;
        }
    }

protected:
    float minThreat;
    int minTankCount;
    int maxDpsCount;
};


Unit* TankTargetValue::Calculate()
{
    FindTargetForTankStrategy strategy(ai);
    return FindTarget(&strategy);
}