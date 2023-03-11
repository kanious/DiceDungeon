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
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();

public:
	// Return current camera position
	glm::vec3 GetCameraPos();
	// Play sound if something collide
	void CollisionSoundCallback();
private:
	// Check User input
	void KeyCheck();
	// Saves camera position
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	// Reset camera position
	void ResetDefaultCameraPos();

private:
	// Initialize
	RESULT Ready(std::string dataPath);
	// Initialize Layer and Camera
	RESULT ReadyLayerAndCamera();
	// Load Objects from json file
	void LoadObjects();
	// Initialize Physics World and prepare dice
	void ReadyPhysicsAndDice();
public:
	// Create an instance
	static SceneDungeon* Create(std::string dataPath);

};

#endif //_SCENEDUNGEON_H_