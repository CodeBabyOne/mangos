#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/shaman/ShamanAiObjectContext.h"

using namespace ai;


class CasterShamanTestCase : public EngineTestBase
{
    CPPUNIT_TEST_SUITE( CasterShamanTestCase );
    CPPUNIT_TEST( combat );
	CPPUNIT_TEST( buff );
	CPPUNIT_TEST( incompatibles );
    CPPUNIT_TEST( aoe );
    CPPUNIT_TEST( snare );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {
		EngineTestBase::setUp();
		setupEngine(new ShamanAiObjectContext(ai), "caster", NULL);

        addAura("water shield");
        addAura("flametongue weapon");
        addTargetAura("flame shock");
    }

protected:
    void combat()
    {
        removeTargetAura("flame shock");

		tick();
        tick();
        addTargetAura("earth shock");

		tick();
		tick();

        assertActions(">T:flame shock>T:earth shock>S:searing totem>T:lightning bolt");
    }

    void buff()
    {
        engine->addStrategy("bmana");
        removeAura("water shield");
        removeAura("flametongue weapon");

        tick();
        addAura("flametongue weapon");

        tick();
        addAura("water shield");

        assertActions(">S:flametongue weapon>S:water shield");
    }

    void incompatibles()
    {
        engine->addStrategies("melee", "dps", "heal", "caster", NULL);

        CPPUNIT_ASSERT(engine->ListStrategies() == "Strategies: caster");
    }

    void aoe()
    {
        engine->addStrategy("caster aoe");

        tickWithAttackerCount(3);
        tickWithAttackerCount(3);
        tickWithAttackerCount(3);
        tickWithAttackerCount(3);
        tickWithAttackerCount(3);

        assertActions(">T:reach melee>S:magma totem>T:thunderstorm>T:fire nova>T:chain lightning");
    }

    void snare()
    {
        tickWithTargetIsMoving();

        assertActions(">T:frost shock");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( CasterShamanTestCase );
