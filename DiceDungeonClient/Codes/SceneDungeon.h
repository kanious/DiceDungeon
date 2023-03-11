#ifndef _SCENEDUNGEON_H_
#define _SCENEDUNGEON_H_

#include "Scene.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CInputDevice;
	class CLayer;
	class CSkyBox;
	class iPhysicsFactory;
	class iPhysicsWorld;
}

class UIManager;
class DefaultCamera;
class BGObject;
class AnimationManager;
class TargetManager;

// A game scene class that inherits from the engine's CScene class
class SceneDungeon : public Engine::CScene
{
private:
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CLayer*				m_pCharacterLayer;
	Engine::CSkyBox*			m_pSkyBox;
	UIManager*					m_pUIManager;
	DefaultCamera*				m_pDefaultCamera;
	AnimationManager*			m_pAnimationManager;
	TargetManager*				m_pTargetManager;

	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

	Engine::iPhysicsFactory*	m_pPFactory;
	Engine::iPhysicsWorld*		m_pPWorld;

private:
	explicit SceneDungeon();
	virtual ~SceneDungeon();
	virtual void Destroy();
public:
	virtual void Update(const _float& dt);
	virtual void Render();

public:
	glm::vec3 GetCameraPos();
	void CollisionSoundCallback();
private:
	void KeyCheck();
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	void ResetDefaultCameraPos();

private:
	RESULT Ready(std::string dataPath);
	RESULT ReadyLayerAndGameObject();
	void LoadObjects();
	void AddDice();
public:
	static SceneDungeon* Create(std::string dataPath);

};

#endif //_SCENEDUNGEON_H_