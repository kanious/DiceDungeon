#ifndef _DEFAULTCAMERA_H_
#define _DEFAULTCAMERA_H_

#include "GameObject.h"

namespace Engine
{
	class CCamera;
	class CInputDevice;
}
class Player;

// Free Camera Class
class DefaultCamera : public Engine::CGameObject
{
private:
	enum eCameraDir
	{
		CD_UP,
		CD_DOWN,
		CD_LEFT,
		CD_RIGHT,
		CD_END
	};
private:
	Engine::CCamera*				m_pCamera;
	Engine::CInputDevice*			m_pInputDevice;

	_bool							m_bMouseEnable;

	_float							m_fAngleX = 0.f;
	_float							m_fAngleY = 0.f;
	_float							m_fSpeed;
	_float							m_fRotSpeed;

	std::vector<_uint>				m_vecShaders;

	Player*							m_pTarget;
	_float							m_fDistance;

	eCameraDir						m_eCurDir;
	eCameraDir						m_ePrevDir;
	glm::vec3						m_vDestPos;
	_float							m_fHeight;
	_float							m_fDist;

	int								m_iZoomLevel;
	int								m_iZoomMinLevel;
	int								m_iZoomMaxLevel;

private:
	explicit DefaultCamera();
	~DefaultCamera();

public:
	_bool GetMouseEnable()						{ return m_bMouseEnable; }
	glm::vec3 GetCameraEye();
	glm::vec3 GetCameraRot();
	glm::vec3 GetCameraTarget();
	void SetMouseEnable(_bool enable);
	void SetCameraEye(glm::vec3 eye);
	void SetCameraRot(glm::vec3 rot);
	void SetCameraTarget(glm::vec3 target);
	void SetPlayer(Player* pPlayer)				{ m_pTarget = pPlayer; }
public:
	// Save shaders to deliver camera information
	void AddShaderLocation(std::string shaderTag);

private:
	// Check user input
	void KeyCheck(const _float& dt);
	// Following Target
	void FollowPlayer();
	// Move Camera to current dir
	void MoveCameraDir(const _float& dt, eCameraDir eDir);
	// Rotate Camera Dir
	void RotateCameraDir(_bool clockwise);

public:
	// Basic Update Function
	virtual void Update(const _float& dt);

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
public:
	// Create an instance
	static DefaultCamera* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
};

#endif //_DEFAULTCAMERA_H_