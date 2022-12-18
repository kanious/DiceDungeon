#ifndef _SCENE3D_H_
#define _SCENE3D_H_

#include "EngineStruct.h"
#include "Scene.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CInputDevice;
	class CLayer;
	class CSkyBox;
}

class UIManager;
class DefaultCamera;
class Player;
class NetworkManager;
class InputInfo;
class GameState;

class Scene3D : public Engine::CScene
{
private:
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CLayer*				m_pObjLayer;
	Engine::CSkyBox*			m_pSkyBox;
	UIManager*					m_pUIManager;
	DefaultCamera*				m_pDefaultCamera;

	NetworkManager*				m_pNetworkManager;
	Player*						m_pPlayer[4];
	InputInfo*					m_pInputInfo;

	//Engine::sUserInput*			m_pPlayerInput;
	Engine::sGameState*			m_pPlayerState;

	_int m_iIndex;

	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

private:
	explicit Scene3D();
	virtual ~Scene3D();
	
private:
	void KeyCheck(const _float& dt);

public:
	void ResetInput();
	void SetGameState(GameState& packet);
private:
	void SetDefaultCameraSavedPosition(glm::vec3 vPos, glm::vec3 vRot, glm::vec3 target);
	void ResetDefaultCameraPos();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static Scene3D* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENE3D_H_