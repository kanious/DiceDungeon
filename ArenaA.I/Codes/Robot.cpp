#include "Robot.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "AABB.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "RobotController.h"
#include "ArenaDefine.h"
#include "ArenaFunction.h"


USING(Engine)
USING(glm)
USING(std)

Robot::Robot()
	: m_pMesh(nullptr), m_pWeaponMesh(nullptr), m_iRobotNumber(0), m_eWeaponType(WEAPON_LASER)
	, m_pTarget(nullptr), m_iHP(100), m_bRetired(false)
	, m_bGlobalCoolTime(true), m_fGlobalCoolTime(0.f), m_fGlobalCoolTimeElapse(0.f)
	, m_bWeaponCoolTime(false), m_fWeaponCoolTime(0.f), m_fWeaponCoolTimeElapse(0.f)
	, m_bWaitingProjectile(false), m_fInitGravity(-0.98f), m_fCurrentGravity(-0.98f)
{
	m_bDebug = false;
	m_pController = RobotController::GetInstance(); m_pController->AddRefCnt();

	int iRand = GetRandNum(500, 1000);
	m_fGlobalCoolTime = iRand / 1000.f;
	m_fGlobalCoolTime = 0.5f;
}

Robot::~Robot()
{
}

void Robot::Update(const _float& dt)
{
	if (m_pController->GetGameOver())
		return;

	if (nullptr == m_pController || nullptr == m_pTransform)
		return;

	if (m_bRetired)
		goto Retired;

	m_bWaitingCoolTime = false;

	if (m_bGlobalCoolTime)
	{
		m_fGlobalCoolTimeElapse += dt;
		if (m_fGlobalCoolTimeElapse >= m_fGlobalCoolTime)
		{
			m_fGlobalCoolTimeElapse = 0.f;
			m_bGlobalCoolTime = false;
			m_bWaitingCoolTime = false;
		}
		else
			m_bWaitingCoolTime = true;
	}

	if (m_bWeaponCoolTime)
	{
		m_fWeaponCoolTimeElapse += dt;
		if (m_fWeaponCoolTimeElapse >= m_fWeaponCoolTime)
		{
			m_fWeaponCoolTimeElapse = 0.f;
			m_bWeaponCoolTime = false;
			if (!m_bGlobalCoolTime)
				m_bWaitingCoolTime = false;
		}
		else
			m_bWaitingCoolTime = true;
	}

	if (m_bWaitingProjectile)
		m_bWaitingCoolTime = true;


	if (!m_bWaitingCoolTime)
	{
		if (nullptr == m_pTarget)
		{
			RequestTarget();
		}
		else
		{
			if (m_eWeaponType == WEAPON_ROCKET)
				RequestShoot();
			else
			{
				if (!m_bTargetConfirmed)
					RequestEyeOnTarget();
				else
					RequestShoot();
			}
		}
	}

Retired:
	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (!m_bRetired)
			m_pWeaponMesh->Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void Robot::Render()
{
	if (!m_bRetired)
		m_pWeaponMesh->Render();

	CGameObject::Render();
}

void Robot::Destroy()
{
	SafeDestroy(m_pController);

	CGameObject::Destroy();
}

RESULT Robot::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string weaponType)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Clone.Mesh
	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox_AABB = m_pMesh->GetBoundingBoxAABB();
		if (nullptr != m_pBoundingBox_AABB)
			m_pBoundingBox_AABB->SetTransform(m_pTransform);
	}

	//Clone.Weapon
	m_pWeaponMesh = CloneComponent<CMesh*>("weapon");
	if (nullptr != m_pWeaponMesh)
	{
		//AttachComponent("WeaponMesh", m_pWeaponMesh);
		m_pWeaponMesh->SetTransform(m_pTransform);
		m_pWeaponMesh->SetTexture(weaponType);
		m_weaponType = weaponType;
		if (m_weaponType == "weapon_laser")
		{
			m_eWeaponType = WEAPON_LASER;
			m_fWeaponCoolTime = 2.f;
		}
		else if (m_weaponType == "weapon_bullet")
		{
			m_eWeaponType = WEAPON_BULLET;
			m_fWeaponCoolTime = 1.2f;
		}
		else if (m_weaponType == "weapon_bomb")
		{
			m_eWeaponType = WEAPON_BOMB;
			m_fWeaponCoolTime = 5.f;
		}
		else if (m_weaponType == "weapon_rocket")
		{
			m_eWeaponType = WEAPON_ROCKET;
			m_fWeaponCoolTime = 10.f;
		}
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

Robot* Robot::GetTarget()
{
	return m_pTarget;
}

void Robot::LostTarget()
{
	m_pTarget = nullptr;
	m_bTargetConfirmed = false;
	m_fCurrentGravity = m_fInitGravity;

	m_bGlobalCoolTime = true;
}

void Robot::Damaged(_int value)
{
	m_iHP -= value;

	if (m_iHP <= 0)
	{
		m_iHP = 0;
		m_bRetired = true;
	
		m_pMesh->SetTexture("robot_dead");
	}
}

void Robot::ProjectileResult(_bool success, _bool closer)
{
	if (m_eWeaponType == WEAPON_BOMB)
	{
		if (!success)
		{
			if (closer)
				m_fCurrentGravity += m_fInitGravity * 0.1f;
			else
				m_fCurrentGravity -= m_fInitGravity * 0.1f;
		}
	}
	else
	{
		if (!success)
			LostTarget();
	}
}

void Robot::RequestTarget()
{
	_int targetNum = m_pController->FindNextTarget(m_iRobotNumber);

	if (-1 == targetNum)
		m_pTransform->SetPosition(m_pController->Teleport(m_iRobotNumber));
	else
		m_pTarget = m_pController->GetRobot(targetNum);

	m_bGlobalCoolTime = true;
}

void Robot::RequestEyeOnTarget()
{
	m_bTargetConfirmed = m_pController->CanISeeTheTarget(this, m_pTarget);

	if (!m_bTargetConfirmed)
	{
		m_pTarget = nullptr;
	}

	m_bGlobalCoolTime = true;
}

void Robot::RequestShoot()
{
	if (nullptr == m_pTarget)
		m_bGlobalCoolTime = true;

	if (m_pController->ShootTheTarget(this, m_pTarget, m_eWeaponType, m_fCurrentGravity))
	{
		m_bWeaponCoolTime = true;
		m_bGlobalCoolTime = true;

		if (m_eWeaponType == WEAPON_BOMB || m_eWeaponType == WEAPON_ROCKET)
			m_bWaitingProjectile = true;
	}
	else
	{
		LostTarget();
	}
}

Robot* Robot::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string weaponType)
{
	Robot* pInstance = new Robot();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, weaponType))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
