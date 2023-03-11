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

// A game scene class that inherits from the engine's CScene class
class Scene3D : public Engine::CScene
{
private:
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CSkyBox*			m_pSkyBox;

	UIManager*					m_pUIManager;

	DefaultCamera*				m_pDefaultCamera;
	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

	Engine::CLayer*				m_pCharacterLayer;

private:
	explicit Scene3D();
	virtual ~Scene3D();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();
	
public:
	glm::vec3 GetCameraPos();
	Engine::CGameObject * GetTarget();

private:
	// Return current camera position
	void KeyCheck();
	// Saves camera position
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	// Reset camera position
	void ResetDefaultCameraPos();

public:
	// Add object
	Engine::CGameObject* AddGameObject(eLAYERTAG tag, std::string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
	// Save Objects
	void SaveBackgroundObjects();
	// Load Objects
	void LoadObjects();
	// Empty all layers
	void ResetAllLayers();

private:
	// Initialize
	RESULT Ready(std::string dataPath);
	// Initialize Layer and GameObjects
	RESULT ReadyLayerAndGameObject();
public:
	// Create an instance
	static Scene3D* Create(std::string dataPath);
};

#endif //_SCENE3D_H_