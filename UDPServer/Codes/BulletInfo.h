#ifndef _BULLETINIFO_H_
#define _BULLETINIFO_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"
#include "Transform.h"

class BulletInfo
{
public:
	_int					m_iState;
	Engine::CTransform*		m_pTransform;

	_float					m_fElaspedTime;
	_float					m_fTime;

public:
	explicit BulletInfo();
	~BulletInfo();

public:
	glm::vec3 GetPosition() { return m_pTransform->GetPosition(); }
	_float GetRotationY() { return m_pTransform->GetRotationY(); }

public:
	void Update(const _float& dt);
};

#endif //_BULLETINIFO_H_