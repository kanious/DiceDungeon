#ifndef _PLAYERINFO_H_
#define _PLAYERINFO_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"
#include "Transform.h"

class BulletInfo;
class PlayerInfo
{
public:
	_int					m_iState;
	Engine::CTransform*		m_pTransform;
	_long					m_tickNumber;

public:
	explicit PlayerInfo();
	~PlayerInfo();

public:
	glm::vec3 GetPosition() { return m_pTransform->GetPosition(); }
	_float GetRotationY() { return m_pTransform->GetRotationY(); }

public:
	void UpdateTransform();
	void Forward(const _float& dt);
	void Backward(const _float& dt);
	void Left(const _float& dt);
	void Right(const _float& dt);
	void TurnLeft(const _float& dt);
	void TurnRight(const _float& dt);
	void Fire(BulletInfo* myBullet);
	void Respawn();
};

#endif //_PLAYERINFO_H_