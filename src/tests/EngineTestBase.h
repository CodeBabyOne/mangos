#pragma once;

#include "../game/playerbot/strategy/Action.h"
#include "../game/playerbot/strategy/ActionBasket.h"
#include "../game/playerbot/strategy/Queue.h"
#include "../game/playerbot/strategy/Trigger.h"
#include "../game/playerbot/strategy/Engine.h"

#include "MockAiSpellManager.h"
#include "MockAiTargetManager.h"
#include "MockAiMoveManager.h"
#include "MockAiInventoryManager.h"
#include "MockAiSocialManager.h" 
#include "MockAiStrategyManager.h" 
#include "MockAiManagerRegistry.h"

using namespace ai;

class EngineTestBase : public CPPUNIT_NS::TestFixture
{
public:
    virtual void setUp();
    virtual void tearDown();

private:
	void va_generic(void (EngineTestBase::*callback)(const char*), va_list vl);

protected:
    template <class T>
    void set(const char* name, T value) { context->GetValue<T>(name)->Set(value); }
    template <class T>
    void set(const char* name, const char* param, T value) { context->GetValue<T>(name, param)->Set(value); }

protected:
	virtual void setupEngine(AiObjectContext* AiObjectContext, ...);
	void setupEngineCallback(const char* name);
	void tick();
	void assertActions(const char* expected);

	void tickWithNoTarget();
	void spellUnavailable(const char* spell);
	void tickWithSpellUnavailable(const char* spell);
	void tickWithSpellAvailable(const char* spell);
	void spellAvailable(const char* spell);
	void addAura(const char* spell);
	void addTargetAura(const char* spell);
	void removeTargetAura(const char* spell);
	void addPartyAura(const char* spell);
	void removePartyAura(const char* spell);
	void removeAura(const char* spell);
	void tickOutOfSpellRange();
	void tickOutOfMeleeRange();
	void tickInMeleeRange();
	void tickInSpellRange();
	void tickWithNoAggro();
	void tickWithRage(int amount);
	void tickWithEnergy(int amount);
	void tickWithAttackerCount(int count);
	void tickWithMyAttackerCount(int count);
	void tickWithLowHealth(int amount);
	void tickWithPartyLowHealth(int amount);
	void tickWithAuraToDispel(uint32 type);
	void tickWithPartyAuraToDispel(uint32 type);
	void tickWithTargetAuraToDispel(uint32 type);
    void tickBehindTarget();
	void lowHealth(int amount);
	void lowMana(int amount);
	void healthRestored();
	void tickWithComboPoints(int amount);
	void tickWithTargetIsCastingNonMeleeSpell();
	void tickWithBalancePercent(int percent);
	void tickWithNoPet();
	void tickWithPetLowHealth(int amount);
    void tickWithPetDead();
	void tickWithLowMana(int amount);
	void tickWithTargetLowHealth(int amount);
	void tickWithTargetIsMoving();
	void tickWithLootAvailable();
	void tickWithNoFood();
	void tickWithNoDrink();
	void itemAvailable(const char* item, int amount);
	void tickWithDeadPartyMember();
    void tickWithCcTarget(const char*  spell);

protected:
    MockAiManagerRegistry *ai;
	MockAiSpellManager* spellManager;
	MockAiTargetManager* targetManager;
	MockAiMoveManager* moveManager;
	MockAiInventoryManager* inventoryManager;
	MockAiSocialManager* socialManager;
    Engine *engine;
    AiObjectContext *context;
};
