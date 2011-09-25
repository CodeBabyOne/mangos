#pragma once

using namespace std;

namespace ai
{
    class MockedTargets
    {
	public:
		static Unit* GetPartyMember() { return (Unit*)1; }
		static Unit* GetCurrentTarget()  { return (Unit*)7; }
		static Player* GetSelf() { return (Player*)9; }
		static Unit* GetPet() { return (Unit*)10; }
		static Player* GetMaster() { return (Player*)2; }
        static Player* GetTargetForDps() { return (Player*)3; }
        static Player* GetTargetForTank() { return (Player*)4; }
        static Player* GetCc() { return (Player*)12; }
		static Player* GetLineTarget() { return (Player*)14; }
		static Unit* GetLeastHpTarget() { return (Unit*)15; }
		static Unit* GetLeastHpPartyMember() { return (Unit*)16; }

        static void Append(string &buffer, Unit* unit)
        {
            if (unit == GetPartyMember())
                buffer.append("P");
            if (unit == GetCurrentTarget())
                buffer.append("T");
            if (unit == GetSelf())
                buffer.append("S");
            if (unit == GetPet())
                buffer.append("Pet");
            if (unit == GetMaster())
                buffer.append("M");
            if (unit == GetTargetForDps())
                buffer.append("Dps");
            if (unit == GetTargetForTank())
                buffer.append("Tank");
            if (unit == GetCc())
                buffer.append("Cc");
            if (unit == GetLineTarget())
                buffer.append("Line");
            if (unit == GetLeastHpTarget())
                buffer.append("LeastHp");
        }
   };

}
