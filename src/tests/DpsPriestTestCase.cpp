#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/priest/PriestAiObjectContext.h"

using namespace ai;


class DpsPriestTestCase : public EngineTestBase
{
    CPPUNIT_TEST_SUITE( DpsPriestTestCase );
    CPPUNIT_TEST( combat );
    CPPUNIT_TEST_SUITE_END();


public:
    void setUp()
    {
		EngineTestBase::setUp();
		setupEngine(new PriestAiObjectContext(ai), "dps", NULL);

        addAura("power word: fortitude");
        addAura("divine spirit");
        addAura("inner fire");
        addPartyAura("power word: fortitude");
        addPartyAura("divine spirit");
    }

protected:
    void combat()
    {
        tick();
        addAura("shadowform");

        tick();
        tick();
        tick();
        tick();
        tick();
        tick();

        tickWithLowHealth(39);
        tickWithLowHealth(39);
        tickWithLowHealth(39);

        spellAvailable("power word: shield");
        spellAvailable("greater heal");
        addAura("shadowform");
        tickWithLowHealth(1);
        tickWithLowHealth(1);
        tickWithLowHealth(1);
        
        assertActions(">S:shadowform>T:devouring plague>T:shadow word: pain>T:vampiric touch>T:mind blast>T:mind flay>T:shoot>S:remove shadowform>S:power word: shield>S:greater heal>S:remove shadowform>S:power word: shield>S:flash heal");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( DpsPriestTestCase );
