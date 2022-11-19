#include "RobotController.h"
#include "Robot.h"
#include "BGObject.h"
#include "CollisionMaster.h"
#include "Layer.h"
#include "GameObject.h"
#include "ArenaFunction.h"
#include "ArenaDefine.h"
#include "Enums.h"
#include "QuadTree.h"
#include "SphereInstance.h"
#include "SphereArc.h"
#include "SphereRocket.h"


SINGLETON_FUNCTION(RobotController)
USING(Engine)
USING(std)
USING(glm)

_bool Compare(TargetInfo* t1, TargetInfo* t2);
#define YCORRECTION 1.f

RobotController::RobotController()
	: m_pQuadTree(nullptr)
{
	m_pCollisionMaster = CCollisionMaster::GetInstance(); m_pCollisionMaster->AddRefCnt();
	m_vecRobots.clear();
	m_vecTargets.clear();
}

RobotController::~RobotController()
{
}

void RobotController::Destroy()
{
	SafeDestroy(m_pCollisionMaster);
	m_vecRobots.clear();
	TARGETS::iterator iter;
	for (iter = m_vecTargets.begin(); iter != m_vecTargets.end(); ++iter)
	{
		vector<TargetInfo*>::iterator iter2;
		for (iter2 = (*iter)->begin(); iter2 != (*iter)->end(); ++iter2)
			delete (*iter2);
		(*iter)->clear();
		delete (*iter);
	}
	m_vecTargets.clear();
	m_pQuadTree = nullptr;
}

void RobotController::Update(const _float& dt)
{
	if (m_bGameOver)
		return;

	_int index = 0;
	for (int i = 0; i < m_vecRobots.size(); ++i)
	{
		Robot* robot = m_vecRobots[i];
		if (robot->GetRetired())
			continue;
		index++;
	}
	if (index <= 1)
	{
		m_bGameOver = true;
		cout << "Game is finished!" << endl;
	}
}

RESULT RobotController::Ready(BGObject* terrain, CLayer* pLayer, _uint num)
{
	if (nullptr != terrain)
	{
		m_pQuadTree = terrain->GetQuadTreeFromMesh();
	}
	m_pLayer = pLayer;

	SpawnRobots(num);
	SetTargetingSystem(num);

	return PK_NOERROR;
}

void RobotController::SpawnRobots(_uint num)
{
	CGameObject* pGameObject = nullptr;

	if (nullptr != m_pLayer)
	{
		vec3 vRayPos = vec3(0.f);
		vec3 vTarget = vec3(0.f);

		if (nullptr != m_pQuadTree)
		{
			vec3 vDest = vec3(0.f);
			for (_uint i = 0; i < num; ++i)
			{
				vRayPos = vec3(GetRandNum(MAP_MINSIZE, MAP_MAXSIZE), 25.f, GetRandNum(MAP_MINSIZE, MAP_MAXSIZE));
				vTarget = vec3(vRayPos.x, -25.f, vRayPos.z);
				if (m_pCollisionMaster->IntersectRayToTriangles(m_pQuadTree, vRayPos, vTarget, vDest))
				{
					int wType = GetRandNum(0, 3);
					string weaponType = "weapon_laser";
					switch (wType)
					{
					case 0:
						weaponType = "weapon_laser";
						break;
					case 1:
						weaponType = "weapon_bullet";
						break;
					case 2:
						weaponType = "weapon_bomb";
						break;
					case 3:
						weaponType = "weapon_rocket";
						break;
					}

					pGameObject = Robot::Create((_uint)SCENE_ARENA, (_uint)LAYER_CHARACTER, (_uint)OBJ_PLAYER, m_pLayer,
						"robot", vDest, vec3(0.f), vec3(1.f), weaponType);
					if (nullptr != pGameObject)
					{
						m_pLayer->AddGameObject(pGameObject);
						AddRobots(dynamic_cast<Robot*>(pGameObject));
					}
				}
			}
		}
	}
}

vec3 RobotController::Teleport(_uint robotNum)
{
	if (nullptr == m_pQuadTree || 0 > robotNum || robotNum >= m_vecTargets.size())
		return vec3(0.f);

	vec3 vDest = m_vecRobots[robotNum]->GetPosition();
	vec3 vPos = vec3(GetRandNum(MAP_MINSIZE - m_iBattleFieldArea, MAP_MAXSIZE - m_iBattleFieldArea)
		, 100.f
		, GetRandNum(MAP_MINSIZE - m_iBattleFieldArea, MAP_MAXSIZE - m_iBattleFieldArea));
	vec3 vTarget = vec3(vPos.x, -100.f, vPos.z);
	if (CCollisionMaster::GetInstance()->IntersectRayToTriangles(m_pQuadTree, vPos, vTarget, vDest))
	{
		ResetTargetStatus(robotNum);
	}

	m_iBattleFieldArea += 1.f;
	if (m_iBattleFieldArea > 50.f)
		m_iBattleFieldArea = 50.f;

	return vDest;
}

Robot* RobotController::GetRobot(_uint robotNum)
{
	if (0 > robotNum || robotNum >= m_vecRobots.size())
		return nullptr;

	return m_vecRobots[robotNum];
}

_int RobotController::FindNextTarget(_uint robotNum)
{
	if (0 > robotNum || robotNum >= m_vecTargets.size())
		return -1;

	vector<TargetInfo*>* pVecTargets = m_vecTargets[robotNum];
	if (nullptr == pVecTargets)
		return -1;

	vec3 vMyPos = m_vecRobots[robotNum]->GetPosition();
	for (int i = 0; i < pVecTargets->size(); ++i)
	{
		if (m_vecRobots[i]->GetRetired())
			continue;
		_uint num = (*pVecTargets)[i]->targetNum;
		vec3 vTargetPos = m_vecRobots[num]->GetPosition();
		(*pVecTargets)[i]->fDistance = distance(vMyPos, vTargetPos);
	}

	sort(pVecTargets->begin(), pVecTargets->end(), Compare);

	_int targetNum = -1;
	for (int i = 0; i < pVecTargets->size(); ++i)
	{
		if (m_vecRobots[i]->GetRetired())
			continue;
		if (!(*pVecTargets)[i]->isChecked)
		{
			(*pVecTargets)[i]->isChecked = true;
			return (*pVecTargets)[i]->targetNum;
		}
	}
	return targetNum;
}

_bool RobotController::CanISeeTheTarget(Robot* main, Robot* target)
{
	if (nullptr == main || nullptr == target)
		return false;

	vec3 vMainPos = main->GetPosition();
	vec3 vTargetPos = target->GetPosition();
	vMainPos.y += YCORRECTION;
	vTargetPos.y += YCORRECTION;

	vec3 vDest(0.f);
	_bool blocked = m_pCollisionMaster->IsRayBlockedByTriangles(m_pQuadTree, vMainPos, vTargetPos, vDest);
	return !blocked;
}

_bool RobotController::ShootTheTarget(Robot* main, Robot* target, eWeaponType type, _float gravity)
{
	if (nullptr == m_pLayer || nullptr == main || nullptr == target)
		return false;

	if (target->GetRetired())
		return false;

	CGameObject* pGameObject = nullptr;
	vec3 vPos = main->GetPosition();
	vec3 vTargetPos = target->GetPosition();
	vPos.y += YCORRECTION;
	vTargetPos.y += YCORRECTION;
	vec3 vDir = vTargetPos - vPos;
	vDir = normalize(vDir);

	switch (type)
	{
	case WEAPON_LASER:
	{
		pGameObject = SphereInstance::Create((_uint)SCENE_ARENA, (_uint)LAYER_CHARACTER, (_uint)OBJ_PLAYER, m_pLayer,
			"sphere", vPos, vec3(0.f), vec3(0.1f), true, vTargetPos);
		if (nullptr == pGameObject)
			return false;
		m_pLayer->AddGameObject(pGameObject);
		target->Damaged(4);
	}
		break;
	case WEAPON_BULLET:
	{
		pGameObject = SphereInstance::Create((_uint)SCENE_ARENA, (_uint)LAYER_CHARACTER, (_uint)OBJ_PLAYER, m_pLayer,
			"sphere", vPos, vec3(0.f), vec3(0.1f), true, vTargetPos);
		if (nullptr == pGameObject)
			return false;
		m_pLayer->AddGameObject(pGameObject);
		target->Damaged(2);
	}
		break;

	case WEAPON_BOMB:
	{
		pGameObject = SphereArc::Create((_uint)SCENE_ARENA, (_uint)LAYER_CHARACTER, (_uint)OBJ_PLAYER, m_pLayer,
			"sphere", vPos, vec3(0.f), vec3(0.5f), main->GetRobotNumber(), vDir, gravity);
		if (nullptr == pGameObject)
			return false;
		m_pLayer->AddGameObject(pGameObject);
		dynamic_cast<SphereArc*>(pGameObject)->SetQuadTree(m_pQuadTree);
	}
		break;

	case WEAPON_ROCKET:
	{
		vTargetPos.y -= YCORRECTION * 0.6f;
		pGameObject = SphereRocket::Create((_uint)SCENE_ARENA, (_uint)LAYER_CHARACTER, (_uint)OBJ_PLAYER, m_pLayer,
			"sphere", vPos, vec3(0.f), vec3(0.3f), main->GetRobotNumber(), vDir);
		if (nullptr == pGameObject)
			return false;
		m_pLayer->AddGameObject(pGameObject);
		dynamic_cast<SphereRocket*>(pGameObject)->SetQuadTree(m_pQuadTree);
	}
		break;
	}

	return true;
}

void RobotController::SetProjectileResult(_int robotNum, _bool value, vec3 vBombPos, _float bombRadius, _int damage)
{
	if (0 > robotNum || robotNum >= m_vecRobots.size())
		return;

	Robot* master = m_vecRobots[robotNum];
	master->SetWaitingProjectileStatus(value);

	Robot* target = master->GetTarget();
	if (nullptr == target)
		return;

	vec3 targetPos = target->GetPosition();
	_float fDist = distance(vBombPos, targetPos);

	if (fDist <= bombRadius)
		master->ProjectileResult(true);
	else
	{
		vec3 masterPos = master->GetPosition();
		fDist = distance(masterPos, vBombPos);
		_float fDist2 = distance(masterPos, targetPos);

		if (fDist > fDist2)
			master->ProjectileResult(false, true);
		else
			master->ProjectileResult(false, false);
	}

	for (int i = 0; i < m_vecRobots.size(); ++i)
	{
		Robot* robot = m_vecRobots[i];
		if (robot->GetRetired())
			continue;

		vec3 robotPos = robot->GetPosition();
		fDist = distance(robotPos, vBombPos);
		if (fDist <= bombRadius)
			robot->Damaged(damage);
	}
}

_bool RobotController::RobotCollisionCheck(_uint masterNum, glm::vec3 projectilePos)
{
	for (int i = 0; i < m_vecRobots.size(); ++i)
	{
		Robot* robot = m_vecRobots[i];
		if (robot->GetRetired())
			continue;
		if (robot->GetRobotNumber() == masterNum)
			continue;

		vec3 robotPos = robot->GetPosition();
		_float fDist = distance(robotPos, projectilePos);
		if (fDist <= 1.5f)
			return true;
	}

	return false;
}

void RobotController::AddRobots(Robot* pRobot)
{
	if (nullptr == pRobot)
		return;

	pRobot->SetRobotNumder(m_vecRobots.size());
	m_vecRobots.push_back(pRobot);
}

void RobotController::SetTargetingSystem(_uint num)
{
	for (_uint i = 0; i < num; ++i)
	{
		vector<TargetInfo*>* pVecTarget = new vector<TargetInfo*>();
		for (_uint j = 0; j < num; ++j)
		{
			if (i == j)
				continue;

			TargetInfo* pInfo = new TargetInfo();
			pInfo->targetNum = j;
			pVecTarget->push_back(pInfo);
		}
		m_vecTargets.push_back(pVecTarget);
	}
	int i = 0;
}

void RobotController::ResetTargetStatus(_uint robotNum)
{
	for (int i = 0; i < m_vecTargets.size(); ++i)
	{
		vector<TargetInfo*>* pVecTargets = m_vecTargets[i];
		if (i == robotNum) // myself
		{
			for (int j = 0; j < pVecTargets->size(); ++j)
			{
				(*pVecTargets)[j]->fDistance = 0.f;
				(*pVecTargets)[j]->isChecked = false;
			}
		}
		else // others
		{
			for (int j = 0; j < pVecTargets->size(); ++j)
			{
				if ((*pVecTargets)[j]->targetNum == robotNum)
					(*pVecTargets)[j]->isChecked = false;
			}
		}
	}

	for (int i = 0; i < m_vecRobots.size(); ++i)
	{
		Robot* robot = m_vecRobots[i];
		if (robot->GetRetired())
			continue;

		Robot* target = robot->GetTarget();
		if (nullptr != target)
		{
			if (target->GetRobotNumber() == robotNum)
				robot->LostTarget();
		}
	}
}

_bool Compare(TargetInfo* t1, TargetInfo* t2)
{
	return t1->fDistance < t2->fDistance;
}
