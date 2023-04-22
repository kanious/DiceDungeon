#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "GameObject.h"
#include "EngineStruct.h"
#include "Enums.h"
#include <queue>

namespace Engine
{
	class CMesh;
	class CInputDevice;
	class CAnimation;
}
class Animator;
class SceneDungeon;
class UIHealthInfo;

// Character class
class Player : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;
	Engine::CInputDevice*		m_pInputDevice;
	SceneDungeon*				m_pScene;
	glm::vec3					m_vOriginPosition;
	glm::vec3					m_vVelocity;
	_float						m_fSpeed;
	_float						m_fRotSpeed;

	Animator*					m_pAnimator;
	std::queue<glm::vec3>		m_queuePos;
	glm::vec3					m_vTargetPos;
	_bool						m_bMoving;
	_bool						m_bMotionEnd;

	UIHealthInfo*				m_pHealthUI;
	_bool						m_bDeath;

	glm::vec3					m_vOriginPos;
	glm::vec3					m_vOriginRot;

private:
	explicit Player();
	virtual ~Player();

public:
	glm::vec3 GetVelocity()					{ return m_vVelocity; }
	glm::vec3 GetOriginPosition()			{ return m_vOriginPosition; }
	Animator* GetAnimator()					{ return m_pAnimator; }
	_bool GetDeath()						{ return m_bDeath; }
	// Notify mesh instance of targeting status
	void SetTarget(_bool value);
	void SetScene(SceneDungeon* pScene)		{ m_pScene = pScene; }
	void SetMotionEnd(_bool value)			{ m_bMotionEnd = value; }
	void SetDeath(_bool value)				{ m_bDeath = value; }
	void StartMoving();
	void Hit();
	void Reset();
	void ChangeAnimation(std::string tag);
	void SetAnimationBlending(_bool value);
private:
	// Check movement
	void MovingCheck(const _float& dt);
	// Check key
	void KeyCheck(const _float& dt);

public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();
private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, SceneDungeon* pScene);
public:
	// Create an instance
	static Player* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, SceneDungeon* pScene);
};


#endif //_PLAYER_H_