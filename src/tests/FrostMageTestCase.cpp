#include "pch.h"

#include "aitest.h"
#include "../game/playerbot/strategy/mage/MageAiObjectContext.h"

using namespace ai;


class FrostMageTestCase : public EngineTestBase
{
  CPPUNIT_TEST_SUITE( FrostMageTestCase );
  CPPUNIT_TEST( combatVsMelee );
  CPPUNIT_TEST( dispel );
  CPPUNIT_TEST( boost );
  CPPUNIT_TEST( interruptSpells );
  CPPUNIT_TEST( cc );
  CPPUNIT_TEST( aoe );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp()
    {
		EngineTestBase::setUp();
		setupEngine(new MageAiObjectContext(ai), "frost", NULL);
    }

protected:
 	void combatVsMelee()
	{
        tick();
        spellAvailable("frostbolt");
        
		tickInMeleeRange();
		tickInMeleeRange();

		spellAvailable("frostbolt");
		tickInSpellRange();
        tick();

		tickWithLowHealth(19);

		assertActions(">T:frostbolt>T:frost nova>S:flee>T:frostbolt>T:shoot>S:ice block");
	}

    void dispel() 
    {
        tick(); 

		tickWithAuraToDispel(DISPEL_CURSE);

		spellAvailable("remove curse");
		tickWithPartyAuraToDispel(DISPEL_CURSE);

        tick(); 
		
		tickWithTargetAuraToDispel(DISPEL_MAGIC);

		assertActions(">T:frostbolt>S:remove curse>P:remove curse on party>T:shoot>T:spellsteal");
    }

    void boost() 
    {
        tick(); // frostbolt

		tickWithBalancePercent(1);

        spellAvailable("frostbolt");
        tick(); // frostbolt

        tick(); // shoot

		assertActions(">T:frostbolt>S:icy veins>T:frostbolt>T:shoot");
    }

    void interruptSpells() 
    {
		tickWithTargetIsCastingNonMeleeSpell();

        tick(); // frostbolt

        assertActions(">T:counterspell>T:frostbolt");
    }

    void cc() 
    {
        tickWithCcTarget("polymorph");

        assertActions(">Cc:polymorph");
    }

   	void aoe() 
	{
		tick();
		tickWithAttackerCount(4);
		tick();

		assertActions(">T:frostbolt>T:blizzard>T:shoot");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( FrostMageTestCase );
