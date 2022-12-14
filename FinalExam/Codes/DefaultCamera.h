#ifndef _DEFAULTCAMERA_H_
#define _DEFAULTCAMERA_H_

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
	_float							m_fSpeed;
	_float							m_fRotSpeed;

	_uint							m_shaderLocation;

private:
	explicit DefaultCamera();
	~DefaultCamera();

public:
	_bool GetMouseEnable()						{ return m_bMouseEnable; }
	glm::vec3 GetCameraEye();
	glm::vec3 GetCameraRot();
	glm::vec3 GetCameraTarget();
	_uint GetShaderLocation(_uint shaderID)		{ return m_shaderLocation; }
	void SetMouseEnable(_bool enable);
	void SetCameraEye(glm::vec3 eye);
	void SetCameraRot(glm::vec3 rot);
	void SetCameraTarget(glm::vec3 target);
	void SetShaderLocation(_uint shaderID);

private:
	void KeyCheck(const _float& dt);
	glm::vec3 CheckBoundary(glm::vec3 vPos);

public:
	virtual void Update(const _float& dt);

private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
public:
	static DefaultCamera* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
};

#endif //_DEFAULTCAMERA_H_