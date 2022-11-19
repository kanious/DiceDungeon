#ifndef _ROBOTCONTROLLER_H_
#define _ROBOTCONTROLLER_H_

#include "Base.h"
#include <list>
#include "EngineStruct.h"
#include "Enums.h"
#include "QuadTree.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CCollisionMaster;
	class CQuadTree;
	class CLayer;
}
class Robot;
class BGObject;
class SceneArena;

class TargetInfo
{
public:
	_uint targetNum;
	_float fDistance;
	_bool isChecked;
	explicit TargetInfo()
		: targetNum(0), fDistance(0.f), isChecked(false)
	{}
	~TargetInfo() {}
};

class RobotController : public Engine::CBase
{
	SINGLETON(RobotController)

private:
	Engine::CCollisionMaster*						m_pCollisionMaster;
	Engine::CQuadTree*								m_pQuadTree;
	Engine::CLayer*									m_pLayer;

	std::vector<Robot*>								m_vecRobots;
	std::vector<std::vector<TargetInfo*>*>			m_vecTargets;
	typedef std::vector<std::vector<TargetInfo*>*>	TARGETS;

	_bool											m_bGameOver;
	_uint											m_iBattleFieldArea = 0;

private:
	explicit RobotController();
	~RobotController();
public:
	void Destroy();
	void Update(const _float& dt);
	RESULT Ready(BGObject* terrain, Engine::CLayer* pLayer, _uint num);

public:
	_bool GetGameOver()								{ return m_bGameOver; }

public:
	void SpawnRobots(_uint num);

	glm::vec3 Teleport(_uint robotNum);
	Robot* GetRobot(_uint robotNum);
	_int FindNextTarget(_uint robotNum);
	_bool CanISeeTheTarget(Robot* main, Robot* target);
	_bool ShootTheTarget(Robot* main, Robot* target, eWeaponType type, _float gravity = 0.f);
	void SetProjectileResult(_int robotNum, _bool value, glm::vec3 vBombPos, _float bombRadius, _int damage);
	_bool RobotCollisionCheck(_uint masterNum, glm::vec3 projectilePos);

private:
	void AddRobots(Robot* pRobot);
	void SetTargetingSystem(_uint num);
	void ResetTargetStatus(_uint robotNum);
};

#endif //_ROBOTCONTROLLER_H_