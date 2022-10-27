#ifndef _DEFAULTCAMERA_H_
#define _DEFAULTCAMERA_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CCamera;
	class CInputDevice;
}

class DefaultCamera : public Engine::CGameObject
{
private:
	Engine::CCamera*				m_pCamera;
	Engine::CInputDevice*			m_pInputDevice;

	_bool							m_bMouseEnable;

	_float							m_fAngleX = 0.f;
	_float							m_fAngleY = 0.f;

private:
	explicit DefaultCamera();
	~DefaultCamera();

public:
	glm::vec3 GetCurrentPos();
	_bool GetMouseEnable()			{ return m_bMouseEnable; }

private:
	void KeyCheck(const _float& dt);

public:
	virtual void Update(const _float& dt);

private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
public:
	static DefaultCamera* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
	virtual CGameObject* Clone();
};

#endif //_DEFAULTCAMERA_H_