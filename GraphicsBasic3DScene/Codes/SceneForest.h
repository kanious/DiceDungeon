#ifndef _SCENEFOREST_H_
#define _SCENEFOREST_H_

#include "Scene.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class UIManager;

class SceneForest : public Engine::CScene
{
private:
	Engine::CInputDevice*			m_pInputDevice;
	Engine::CLayer*					m_pBackgroundLayer;
	DefaultCamera*					m_pDefaultCamera;
	UIManager*						m_pUIManager;
	glm::vec3						m_vCameraSavedPos;
	glm::vec3						m_vCameraSavedRot;
	glm::vec3						m_vCameraSavedTarget;

private:
	explicit SceneForest();
	virtual ~SceneForest();

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
	RESULT Ready();
	RESULT ReadyComponent();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneForest* Create();
	virtual void SaveBackgroundObjects();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENEFOREST_H_