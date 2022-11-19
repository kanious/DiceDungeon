#ifndef _SCENEPIRATE_H_
#define _SCENEPIRATE_H_

#include "Scene.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class TargetCamera;
class UIManager;
class BGObject;

class ScenePirate : public Engine::CScene
{
private:
	Engine::CInputDevice*			m_pInputDevice;
	Engine::CLayer*					m_pBackgroundLayer;
	DefaultCamera*					m_pDefaultCamera;
	TargetCamera*					m_pTargetCamera;
	UIManager*						m_pUIManager;
	glm::vec3						m_vCameraSavedPos;
	glm::vec3						m_vCameraSavedRot;
	glm::vec3						m_vCameraSavedTarget;

	BGObject*						m_pShip;
	glm::vec3						m_vStartingPosition;
	glm::vec3						m_vMiddlePosition;
	glm::vec3						m_vDockPosition;
	glm::vec3						m_vLeavingPosition;
	_bool							m_bTargetCameraMode;
	BGObject*						m_pSmallFactory;

	_bool							m_bArriving;

	glm::mat4x4						m_matLighthouse;
	_float							m_fLightAngle;

private:
	explicit ScenePirate();
	virtual ~ScenePirate();

private:
	void KeyCheck(const _float& dt);

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

public:
	DefaultCamera* GetDefaultCamera()					{ return m_pDefaultCamera; }
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	void ResetDefaultCameraPos();
private:
	void SetDayTimeScene();
	void SetNightTimeScene();
	void SetShipMovingScene();
	void SetDefaultCameraMode();
	void SetTargetCameraMode();
	void SetTargetCameraInfo();
	void SetLightHouseLight(const _float& dt);
	
private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static ScenePirate* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENEPIRATE_H_