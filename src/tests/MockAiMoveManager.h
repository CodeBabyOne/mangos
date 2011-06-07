#pragma once

#include "string"

using namespace std;

#include "MockedTargets.h"

namespace ai
{
    class MockAiMoveManager : public AiMoveManager
    {
    public:
        MockAiMoveManager(PlayerbotAI* ai, AiManagerRegistry* aiRegistry, string *buffer) : AiMoveManager(ai, aiRegistry) 
        {
			distanceTo[MockedTargets::GetCurrentTarget()] = 15.0f; 
			moving[MockedTargets::GetCurrentTarget()] = false;
            isBehind[MockedTargets::GetCurrentTarget()] = false;
			this->buffer = buffer;
		}

	public:
		virtual float GetDistanceTo(Unit* target);
		virtual void MoveTo(Unit* target, float distance = 0.0f);
		virtual float GetFollowAngle();
		virtual void Follow(Unit* target, float distance = 2.0f);
		virtual void Follow(Unit* target, float distance, float angle);
		virtual bool Flee(Unit* target, float distance = SPELL_DISTANCE);
		virtual void Stay();
		virtual bool IsMoving(Unit* target);
		virtual void Attack(Unit* target);
        virtual bool IsBehind(Unit* target);
		virtual void StayCircle(float range = 2.0f);
		virtual void StayLine(float range = 2.0f);
		virtual void StayCombat(float range = 2.0f);

	private:
		string *buffer;

	public:
		map<Unit*, float> distanceTo;
		map<Unit*, bool> moving;
        map<Unit*, bool> isBehind;
    };

}