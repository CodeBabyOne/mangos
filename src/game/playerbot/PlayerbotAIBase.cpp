#include "../pchdef.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"

using namespace ai;
using namespace std;

PlayerbotAIBase::PlayerbotAIBase() : nextAICheckDelay(0)
{
}

void PlayerbotAIBase::UpdateAI(uint32 elapsed)
{
    if (nextAICheckDelay > elapsed)
        nextAICheckDelay -= elapsed;
    else
        nextAICheckDelay = 0;

    if (!CanUpdateAI())
        return;

    UpdateAIInternal(elapsed);
    YieldThread();
}

void PlayerbotAIBase::SetNextCheckDelay(const uint32 delay)
{
    nextAICheckDelay = delay;
}

void PlayerbotAIBase::IncreaseNextCheckDelay(uint32 delay)
{
    nextAICheckDelay += delay;
}

bool PlayerbotAIBase::CanUpdateAI()
{
    return nextAICheckDelay <= 100;
}

void PlayerbotAIBase::YieldThread()
{
    if (nextAICheckDelay < sPlayerbotAIConfig.reactDelay)
        nextAICheckDelay = sPlayerbotAIConfig.reactDelay;
}
