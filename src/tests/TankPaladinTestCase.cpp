#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/paladin/PaladinAiObjectContext.h"

using namespace ai;


class TankPaladinTestCase : public EngineTestBase
{
    CPPUNIT_TEST_SUITE( TankPaladinTestCase );
		CPPUNIT_TEST( combatVsMelee );
		CPPUNIT_TEST( paladinMustHoldAggro );
		CPPUNIT_TEST( healing );
		CPPUNIT_TEST( stopEnemyMove );
		CPPUNIT_TEST( buff );
		CPPUNIT_TEST( bmana );
		CPPUNIT_TEST( curePoison );
		CPPUNIT_TEST( cureMagic );
		CPPUNIT_TEST( cureDisease );
		CPPUNIT_TEST( interruptSpells );
		CPPUNIT_TEST( resistances );
		CPPUNIT_TEST( combatIncompatibles );
		CPPUNIT_TEST( buffIncompatibles );
		CPPUNIT_TEST( resistanceIncompatibles );
    CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp()
	{
		EngineTestBase::setUp();
		setupEngine(new PaladinAiObjectContext(ai), "tank", NULL);
		engine->addStrategy("barmor");

        addAura("devotion aura");
        addAura("seal of justice");
        addAura("blessing of sanctuary");
        addAura("righteous fury");

		addAura("holy shield");

        set<float>("distance", "current target", 0.0f);
    }

protected:
    void bmana()
    {
		engine->removeStrategy("bhealth");
		engine->addStrategy("bmana");

        removeAura("seal of justice");
        tick();

		assertActions(">S:seal of wisdom");
	}

    void buff()
    {
        removeAura("devotion aura");
        removeAura("righteous fury");
        removeAura("blessing of sanctuary");
        removeAura("seal of justice");
        removeAura("holy shield");

        tick();
        addAura("devotion aura");

        tick();
        tick();
        addAura("seal of light");

        tick();
        tick();
        addAura("blessing of kings");

        tick();
        addAura("holy shield");

        tick();
        addAura("righteous fury");

        assertActions(">S:devotion aura>S:seal of light>S:seal of justice>S:blessing of sanctuary>S:blessing of kings>S:holy shield>S:righteous fury");
    }

    void healing()
    {
		tickWithLowHealth(50);

		tickWithLowHealth(30);
        tickWithLowHealth(30);

		tickWithLowHealth(19);

		spellAvailable("lay on hands");
		tickWithPartyLowHealth(20);
        tickWithLowHealth(19);
        spellAvailable("flash of light");
        tickWithLowHealth(19);

        assertActions(">S:flash of light>S:divine protection>S:holy light>S:lay on hands>P:lay on hands on party>S:divine shield>S:flash of light");
    }

    void paladinMustHoldAggro()
    {
		tickWithAttackerCount(2);
		tickWithAttackerCount(2);

		tickWithAttackerCount(3);
		tickWithAttackerCount(3);

		tickWithNoAggro();
		tickWithNoAggro();

		assertActions(">T:hammer of the righteous>T:avenger's shield>T:consecration>T:holy wrath>T:hand of reckoning>T:judgement of justice");
    }

    void combatVsMelee()
    {
		tickOutOfMeleeRange();

        tick();
        tick();

		tickWithTargetLowHealth(19);

        assertActions(">T:reach melee>T:melee>T:judgement of light>T:hammer of wrath");
    }

    void stopEnemyMove()
    {
		tickWithTargetIsMoving();

		tick();
		tick();

        assertActions(">T:hammer of justice>T:melee>T:judgement of light");
    }

	void interruptSpells()
	{
		tickWithTargetIsCastingNonMeleeSpell();

		tick();

		assertActions(">T:hammer of justice>T:melee");
	}

    void cureDisease()
    {
        cureKind(DISPEL_DISEASE);
		assertActions(">S:cleanse>P:cleanse disease on party>S:purify>P:purify disease on party");
    }

	void curePoison()
	{
		cureKind(DISPEL_POISON);
		assertActions(">S:cleanse>P:cleanse poison on party>S:purify>P:purify poison on party");
	}

	void cureMagic()
	{
		spellAvailable("cleanse");
		tickWithAuraToDispel(DISPEL_MAGIC);

		spellAvailable("cleanse");
		tickWithPartyAuraToDispel(DISPEL_MAGIC);

		assertActions(">S:cleanse>P:cleanse magic on party");
	}

    void cureKind(DispelType type)
    {
        spellAvailable("cleanse");
        tickWithAuraToDispel(type);

        spellAvailable("cleanse");
        tickWithPartyAuraToDispel(type);

        spellAvailable("purify");
        tickWithAuraToDispel(type);

        spellAvailable("purify");
        tickWithPartyAuraToDispel(type);
    }

	void resistances()
	{
		engine->addStrategy("rshadow");
		tick();

		engine->addStrategy("rfrost");
		tick();

		engine->addStrategy("rfire");
		tick();

		assertActions(">S:shadow resistance aura>S:frost resistance aura>S:fire resistance aura");
	}

    void combatIncompatibles()
    {
        engine->removeStrategy("barmor");
        engine->removeStrategy("tank");
        engine->addStrategies("dps", "tank", NULL);

        CPPUNIT_ASSERT(engine->ListStrategies() == "Strategies: tank");
    }

    void buffIncompatibles()
    {
        engine->removeStrategy("barmor");
        engine->removeStrategy("tank");
        engine->addStrategies("bhealth", "bmana", "bdps", "barmor", NULL);

        CPPUNIT_ASSERT(engine->ListStrategies() == "Strategies: barmor");
    }

    void resistanceIncompatibles()
    {
        engine->removeStrategy("barmor");
        engine->removeStrategy("tank");
        engine->addStrategies("rshadow", "rfrost", "rfire", NULL);

        CPPUNIT_ASSERT(engine->ListStrategies() == "Strategies: rfire");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( TankPaladinTestCase );
