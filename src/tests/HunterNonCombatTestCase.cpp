#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/hunter/HunterAiObjectContext.h"

using namespace ai;


class HunterNonCombatTestCase : public EngineTestBase
{
    CPPUNIT_TEST_SUITE( HunterNonCombatTestCase );
    CPPUNIT_TEST( buff );
    CPPUNIT_TEST( summonPet );
	CPPUNIT_TEST( buffIfPackUnavailable );
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {
		EngineTestBase::setUp();
		setupEngine(new HunterAiObjectContext(ai), "nc", NULL);

        statsManager->attackerCount = 0;
    }

protected:
    void buff()
    {
		engine->addStrategy("bspeed");

		tick();

		tickWithAttackerCount(1);
        
		assertActions(">S:aspect of the pack>S:aspect of the hawk");
    }

    void summonPet()
    {
        tickWithNoPet();
        
		tickWithPetLowHealth(0); // dead
		tickWithPetLowHealth(30);

		assertActions(">S:call pet>S:revive pet>Pet:mend pet");
	}        

    void buffIfPackUnavailable()
    {
		engine->addStrategy("bspeed");

        tickWithSpellUnavailable("aspect of the pack");

		assertActions(">S:aspect of the cheetah");
    }
    
};

CPPUNIT_TEST_SUITE_REGISTRATION( HunterNonCombatTestCase );
