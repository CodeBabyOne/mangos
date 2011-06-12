#include "pch.h"
#include "aitest.h"
#include "../game/playerbot/strategy/druid/DruidAiObjectContext.h"

using namespace ai;

class BearTankDruidTestCase : public EngineTestBase
{
    CPPUNIT_TEST_SUITE( BearTankDruidTestCase );
    CPPUNIT_TEST( druidMustDoMauls );
    CPPUNIT_TEST( combatVsMelee );
    CPPUNIT_TEST( druidMustHoldAggro );
    CPPUNIT_TEST( druidMustDemoralizeAttackers );
    CPPUNIT_TEST( bearFormIfDireNotAvailable );
    CPPUNIT_TEST( healHimself );
    CPPUNIT_TEST( intensiveHealing );
    CPPUNIT_TEST( healOthers );
    CPPUNIT_TEST( curePoison );
    CPPUNIT_TEST( interruptSpells );
    CPPUNIT_TEST( buff );
    CPPUNIT_TEST( cc );
	CPPUNIT_TEST( aoe );
    CPPUNIT_TEST_SUITE_END();


public:
    virtual void setUp()
    {
		EngineTestBase::setUp();
		setupEngine(new DruidAiObjectContext(ai), "bear", NULL);

		addAura("thorns");
        addTargetAura("faerie fire (feral)");
    }

protected:
    void bearFormIfDireNotAvailable()
    {
		tickWithSpellUnavailable("dire bear form");

		assertActions(">S:bear form");
    }

    void druidMustDemoralizeAttackers()
    {
        tick(); 

		spellAvailable("dire bear form"); // aura not yet applied

		tickWithAttackerCount(3);
		addAura("dire bear form");
		tickWithAttackerCount(3);

		tickInMeleeRange();
        
		tick(); 

		assertActions(">S:dire bear form>S:dire bear form>reach melee>T:swipe (bear)>T:demoralizing roar");
    }

    void druidMustHoldAggro()
    {
        tick();
        addAura("dire bear form");

		tickWithNoAggro();

        tick();
        tick();

		assertActions(">S:dire bear form>T:growl>T:feral charge - bear>melee");
    }

    void druidMustDoMauls()
    {
        tick();
        addAura("dire bear form");

		tickInMeleeRange();

		tickWithRage(21);
		tickWithRage(21);
        
		tickWithSpellAvailable("maul");
    
		assertActions(">S:dire bear form>melee>T:mangle (bear)>T:maul>melee");
    }

    void combatVsMelee()
    {
        removeTargetAura("faerie fire (feral)");
        tick();
        tick();
        addAura("dire bear form");

		tickOutOfMeleeRange();
		tick();
		
		tickOutOfMeleeRange();
        tick();

		tickWithRage(41);

		tickWithRage(61);

        tickWithRage(61);
        
        assertActions(">S:dire bear form>T:feral charge - bear>T:faerie fire (feral)>melee>reach melee>melee>T:mangle (bear)>T:maul>T:swipe (bear)");
    }

    void healHimself()
    {
        tick();
        addAura("dire bear form");

		tickInMeleeRange();

		tickWithLowHealth(59);
		tickWithLowHealth(59);

        tick();
		addAura("bear form");
        tick();
                
		tickWithLowHealth(39);
		tickWithLowHealth(39);
        tickWithLowHealth(39);
        
        assertActions(">S:dire bear form>melee>-dire bear form>S:regrowth>S:bear form>melee>-bear form>S:rejuvenation>S:healing touch");
    }

    void intensiveHealing()
    {
        addAura("dire bear form");
		tickWithLowHealth(1);

		assertActions(">S:survival instincts");
    }

    void healOthers()
    {
        tick();
        addAura("dire bear form");

		tickWithPartyLowHealth(59);
		tickWithPartyLowHealth(59);
        tickWithPartyLowHealth(39);
        tickWithPartyLowHealth(39);

		assertActions(">S:dire bear form>-dire bear form>P:regrowth>P:rejuvenation>P:healing touch");
    }
    void curePoison() 
    {
        tick();
        addAura("dire bear form");

		tickWithAuraToDispel(DISPEL_POISON);
		tickWithAuraToDispel(DISPEL_POISON);

		spellAvailable("abolish poison");
		tickWithPartyAuraToDispel(DISPEL_POISON);
		
		tickWithAuraToDispel(DISPEL_POISON);

		spellAvailable("cure poison");
		tickWithPartyAuraToDispel(DISPEL_POISON);

		assertActions(">S:dire bear form>-dire bear form>S:abolish poison>P:abolish poison>S:cure poison>P:cure poison");
    }
    void interruptSpells() 
    {
        tick();
        addAura("dire bear form");
        
		tickInMeleeRange();

		tickWithTargetIsCastingNonMeleeSpell();

        assertActions(">S:dire bear form>melee>T:bash");
    }
	void buff() 
	{
        removeAura("thorns");
        tick();
        
        removeTargetAura("faerie fire (feral)");
        tickInMeleeRange();
        tick();

        assertActions(">S:thorns>S:dire bear form>T:faerie fire (feral)");
	}

    void cc()
    {
        addAura("dire bear form");
        tickWithCcTarget("entangling roots");
        tickWithCcTarget("entangling roots");

        assertActions(">-dire bear form>Cc:entangling roots");
    }

    void aoe()
    {
        addAura("dire bear form");
		tickInMeleeRange();
		tickWithAttackerCount(3);

		assertActions(">melee>T:swipe (bear)");
    }

};

CPPUNIT_TEST_SUITE_REGISTRATION( BearTankDruidTestCase );
