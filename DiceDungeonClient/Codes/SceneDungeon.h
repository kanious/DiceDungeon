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
class Player;
class AnimationManager;
class TargetManager;
class EnemyManager;

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
	EnemyManager*				m_pEnemyManager;

	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

	Engine::iPhysicsFactory*	m_pPFactory;
	Engine::iPhysicsWorld*		m_pPWorld;

	Player*						m_pPlayer;
	_int						m_iTileIdx;
	_bool						m_bPicked;

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
	_bool GetPlayerPos(glm::vec3& playerPos);
	_bool GetPlayerTileIdx(_int& tileIdx);
	Player* GetPlayer()					{ return m_pPlayer; }
	DefaultCamera* GetCamera()			{ return m_pDefaultCamera; }
	// Play sound if something collide
	void CollisionSoundCallback();
	void ResetPicking();
	void MovePlayer();
	void HitPlayer();
	void StartEnemyTurn();
	void StopEnemyTurn();
	void GameOver();
	void GameReset();
private:
	// Check User input
	void KeyCheck();
	// Saves camera position
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	// Reset camera position
	void ResetDefaultCameraPos();
	_int TilePicking();

private:
	// Initialize
	RESULT Ready(std::string dataPath);
	// Initialize Layer and Camera
	RESULT ReadyLayerAndCamera();
	// Load Objects from json file
	void LoadObjects();
	// Load Tile data from json file
	void LoadTiles();
	// Initialize Physics World and prepare dice
	void ReadyPhysicsAndDice();
public:
	// Create an instance
	static SceneDungeon* Create(std::string dataPath);

};

#endif //_SCENEDUNGEON_H_