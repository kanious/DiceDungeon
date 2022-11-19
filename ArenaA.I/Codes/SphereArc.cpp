#include "SphereArc.h"
#include "Renderer.h"
#include "ComponentMaster.h"
#include "CollisionMaster.h"
#include "Layer.h"

#include "Mesh.h"
#include "AABB.h"
#include "Component.h"
#include "Transform.h"
#include "RobotController.h"


USING(Engine)
USING(glm)
USING(std)

SphereArc::SphereArc()
	: m_pMesh(nullptr), m_iMasterNum(0)
{
	m_fGravity = -0.98f;
	m_vVelocity = vec3(20.f);
	m_fAngle = radians(55.f);
}

SphereArc::~SphereArc()
{
}

_bool SphereArc::RobotCollisionCheck()
{
	vec3 vPos = m_pTransform->GetPosition();

	return RobotController::GetInstance()->RobotCollisionCheck(m_iMasterNum, vPos);
}

_bool SphereArc::CollisionCheck()
{
	if (nullptr == m_pTransform)
		return true;

	vec3 vPos = m_pTransform->GetPosition();
	vec3 vRayPos = vec3(vPos.x, 25.f, vPos.z);
	vec3 vTarget = vec3(vPos.x, -25.f, vPos.z);

	if (nullptr != m_pQuadTree)
	{
		if (CCollisionMaster::GetInstance()->IntersectCheckForProjectiles(m_pQuadTree, vRayPos, vTarget, m_vBombPos))
		{
			if (m_vBombPos.y >= vPos.y)
				return true;
			else
				return false;
		}
	}

	//just in case
	if (m_vBombPos.y < -25.f)
		return true;

	return false;
}

void SphereArc::Update(const _float& dt)
{
	if (m_bDead)
		return;

	if (RobotCollisionCheck())
	{
		RobotController::GetInstance()->SetProjectileResult(m_iMasterNum, false, m_vBombPos, 5.f, 25);
		m_bDead = true;
		return;
	}
	else if (CollisionCheck())
	{
		RobotController::GetInstance()->SetProjectileResult(m_iMasterNum, false, m_vBombPos, 5.f, 25);
		m_bDead = true;
		return;
	}

	m_fTimeElapsed += dt;
	m_vVelocity += dt * 10.f;

	// calculate the moved distance of the each direction
	_float xDistance = cos(m_fAngle) * m_vVelocity.x * dt;
	_float yDistance = sin(m_fAngle) * m_vVelocity.y * dt + (m_fGravity * m_fTimeElapsed * m_fTimeElapsed / 2.f);
	_float zDistance = cos(m_fAngle) * m_vVelocity.z * dt;

	m_pTransform->AddPosition(vec3(m_vDir.x * xDistance, yDistance, m_vDir.z * zDistance));

	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void SphereArc::Render()
{
	CGameObject::Render();
}

void SphereArc::Destroy()
{
	CGameObject::Destroy();
}

RESULT SphereArc::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint masterNum, vec3 vDir, _float gravity)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_iMasterNum = masterNum;
	m_vDir = vDir;
	m_fGravity = gravity;

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

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

SphereArc* SphereArc::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint masterNum, vec3 vDir, _float gravity)
{
	SphereArc* pInstance = new SphereArc();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, masterNum, vDir, gravity))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
