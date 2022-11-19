#ifndef _TARGETCAMERA_H_
#define _TARGETCAMERA_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CCamera;
	class CInputDevice;
}

class TargetCamera : public Engine::CGameObject
{
private:
	Engine::CCamera*				m_pCamera;
	Engine::CInputDevice*			m_pInputDevice;

	_bool							m_bMouseEnable;

	_float							m_fAngleX = 0.f;
	_float							m_fAngleY = 0.f;
	_float							m_fSpeed = 40.f;

	_uint							m_shaderLocation;

private:
	explicit TargetCamera();
	~TargetCamera();

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

public:
	virtual void Update(const _float& dt);

private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
public:
	static TargetCamera* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
};

#endif //_TARGETCAMERA_H_