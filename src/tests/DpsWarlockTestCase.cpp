#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/warlock/WarlockAiObjectContext.h"

using namespace ai;


class DpsWarlockTestCase : public EngineTestBase
{
  CPPUNIT_TEST_SUITE( DpsWarlockTestCase );
      CPPUNIT_TEST( combatVsMelee );
      CPPUNIT_TEST( summonPet );
      CPPUNIT_TEST( aoe );
      CPPUNIT_TEST( low_mana );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {
        EngineTestBase::setUp();
        setupEngine(new WarlockAiObjectContext(ai), "dps", NULL);
    }

protected:
 	void combatVsMelee()
	{
 	    tick();
 	    addTargetAura("immolate");

        tick(); 
        addTargetAura("corruption");
        tick(); 
        addTargetAura("curse of agony");

        tick();
        
        tickWithLowHealth(49);

        tick(); 

		tickWithTargetLowHealth(19);

        set<uint8>("item count", "soul shard", 2);
		spellAvailable("drain soul");
		tick();

		spellAvailable("shadow bolt");
		addAura("shadow trance");
		tick();

        spellAvailable("shadow bolt");
		addAura("backlash");
		tick();

		assertActions(">T:immolate>T:corruption>T:curse of agony>T:incinirate>T:drain life>T:shadow bolt>T:drain soul>T:shoot>T:shadow bolt>T:shadow bolt");
	}

    void summonPet()
    {
        tickWithNoPet();

		assertActions(">S:summon imp");
    }

    void low_mana()
    {
        tickWithLowMana(1);

		assertActions(">S:life tap");
    }

    void aoe()
    {
        engine->addStrategy("aoe");

        tickWithAttackerCount(2);
        tickWithAttackerCount(3);
        tickWithAttackerCount(4);

		assertActions(">T:shadowfury>T:seed of corruption>T:rain of fire");
    }
};

CPPUNIT_TEST_SUITE_REGISTRATION( DpsWarlockTestCase );
