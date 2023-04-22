#include "StateMachine.h"
#include "OpenGLDefines.h"
#include "Player.h"


USING(Engine)
USING(std)
USING(glm)

Player* StateMachine::g_Player = nullptr;
void StateMachine::SetPlayer(Player* pPlayer)
{
	g_Player = pPlayer;
}

eAIState StateMachine::GetState(vec3 vEnemyPos, vec3 vPlayerPos, eEnemyType eType)
{
	if (g_Player->GetDeath())
		return STATE_IDLE;

	_float fDist = distance(vEnemyPos, vPlayerPos);

	if (ENEMY_CRAWLER == eType)
	{
		if (15.f > fDist)
			return STATE_ATTACK;
		else
			return STATE_IDLE;
	}

	if (7.5f < fDist)
		return STATE_CHASE;
	else
		return STATE_ATTACK;
}