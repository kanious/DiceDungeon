#ifndef _ENEMY_H_
#define _ENEMY_H_

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

// Enemy class
class Enemy : public Engine::CGameObject
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
	std::string					m_idle;
	std::string					m_run;
	std::string					m_attack;
	std::string					m_death;

	eEnemyType					m_eEnemyType;
	eAIState					m_eAIState;

	_bool						m_bMotion;
	_bool						m_bMotionEnd;
	_bool						m_bRotate;

	UIHealthInfo*				m_pHealthUI;

	glm::vec3					m_vOriginPos;
	glm::vec3					m_vOriginRot;

private:
	explicit Enemy();
	virtual ~Enemy();

public:
	glm::vec3 GetVelocity()					{ return m_vVelocity; }
	glm::vec3 GetOriginPosition()			{ return m_vOriginPosition; }
	Animator* GetAnimator()					{ return m_pAnimator; }
	// Notify mesh instance of targeting status
	void SetTarget(_bool value);
	void SetScene(SceneDungeon* pScene)		{ m_pScene = pScene; }
	void SetMotionEnd(_bool value)			{ m_bMotionEnd = value; }
	void StartMoving();
	void EndTurn();
	void RunAI();
	void Reset();
private:
	void Idle();
	void ChasePlayer();
	void SeePlayer(const _float& dt);
	void AttackPlayer();

private:
	// Check movement
	void MovingCheck(const _float& dt);

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
	static Enemy* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, SceneDungeon* pScene);
};


#endif //_ENEMY_H_