#ifndef _SCENE3D_H_
#define _SCENE3D_H_

#include "Scene.h"
#include "Enums.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CInputDevice;
	class CLayer;
	class CSkyBox;
}

class DefaultCamera;

class UIManager;
class AnimationManager;

class Scene3D : public Engine::CScene
{
private:
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CSkyBox*			m_pSkyBox;

	UIManager*					m_pUIManager;
	AnimationManager*			m_pAnimationManager;

	DefaultCamera*				m_pDefaultCamera;
	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

	Engine::CLayer*				m_pCharacterLayer;

private:
	explicit Scene3D();
	virtual ~Scene3D();
	virtual void Destroy();
public:
	virtual void Update(const _float& dt);
	virtual void Render();
	
public:
	glm::vec3 GetCameraPos();
	Engine::CGameObject * GetTarget();

private:
	void KeyCheck();
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	void ResetDefaultCameraPos();

public:
	void AddGameObject(eLAYERTAG tag, std::string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
	void SaveBackgroundObjects();
	void LoadBackgroundObjects();
	void ResetAllLayers();

private:
	RESULT Ready(std::string dataPath);
	RESULT ReadyLayerAndGameObject();
public:
	static Scene3D* Create(std::string dataPath);
};

#endif //_SCENE3D_H_