#ifndef _SCENEDUNGEON_H_
#define _SCENEDUNGEON_H_

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
class BGObject;
class AnimationManager;

class SceneDungeon : public Engine::CScene
{
private:
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CLayer*				m_pObjLayer;
	Engine::CSkyBox*			m_pSkyBox;
	UIManager*					m_pUIManager;
	DefaultCamera*				m_pDefaultCamera;
	AnimationManager*			m_pAnimationManager;

	glm::vec3					m_vCameraSavedPos;
	glm::vec3					m_vCameraSavedRot;
	glm::vec3					m_vCameraSavedTarget;

	_bool						m_bFollowingMode;

private:
	explicit SceneDungeon();
	virtual ~SceneDungeon();
	
private:
	void KeyCheck();
public:
	void AddBGObject(std::string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, int dir = 0);

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
	void AddCharacters();
public:
	static SceneDungeon* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENEDUNGEON_H_