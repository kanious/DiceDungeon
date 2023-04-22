#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "EngineDefines.h"
#include "Enums.h"
#include "glm\vec3.hpp"

class Player;

class StateMachine
{
public:
	static Player* g_Player;

public:
	static void SetPlayer(Player* pPlayer);
	static eAIState GetState(glm::vec3 vEnemyPos, glm::vec3 vPlayerPos, eEnemyType eType);
};

#endif //_STATEMACHINE_H_