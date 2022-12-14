#ifndef _PHYSICSSYSTEM_H_
#define _PHYSICSSYSTEM_H_

#include "Base.h"
#include "glm\vec3.hpp"
#include "Enums.h"

namespace Engine
{
	class CInputDevice;
}
class SceneFPS;
class BGObject;
class Target;
class DefaultCamera;

class PhysicsSystem : public Engine::CBase
{
	SINGLETON(PhysicsSystem)
private:

private:
	Engine::CInputDevice*		m_pInputDevice;
	SceneFPS*					m_pScene;
	eGameMode					m_eGameMode;
	std::vector<BGObject*>		m_vecBGs;
	std::vector<Target*>		m_vecTargets;
	Target*						m_pHoverTarget;
	DefaultCamera*				m_pDefaultCamera;

	_uint						m_iKillCount;
	_uint						m_iMissCount;

private:
	explicit PhysicsSystem();
	~PhysicsSystem();

public:
	void AddBGObject(BGObject* obj)		{ m_vecBGs.push_back(obj); }
	void AddTarget(Target* obj)			{ m_vecTargets.push_back(obj); }
	_uint GetKillCount()				{ return m_iKillCount; }
	Target* GetHoverTarget()			{ return m_pHoverTarget; }
	void AddKillCount()					{ ++m_iKillCount; }
	void SetGameMode(eGameMode mode)	{ m_eGameMode = mode; }
	eGameMode GetGameMode()				{ return m_eGameMode; }
	void CollisionCheck();
	void GetRandomMovingInfo(_float& fTime, _float& fAccel, glm::vec3& vTargetPos);

private:
	_bool CheckDir(glm::vec3 vCameraPos, glm::vec3 vTargetPos);

public:
	void Destroy();
	void Update(const _float& dt);
	RESULT Ready(SceneFPS* pScene, DefaultCamera* pCamera);
};

#endif //_PHYSICSSYSTEM_H_