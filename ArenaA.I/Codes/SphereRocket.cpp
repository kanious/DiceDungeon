#include "SphereRocket.h"
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

SphereRocket::SphereRocket()
	: m_pMesh(nullptr), m_iMasterNum(0)
{
}

SphereRocket::~SphereRocket()
{
}

_bool SphereRocket::RobotCollisionCheck()
{
	vec3 vPos = m_pTransform->GetPosition();

	return RobotController::GetInstance()->RobotCollisionCheck(m_iMasterNum, vPos);
}

_bool SphereRocket::CollisionCheck()
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
	if (m_vBombPos.y < -25.f || m_vBombPos.y > 25.f || m_vBombPos.x > 70.f || m_vBombPos.x < -70.f
		|| m_vBombPos.z > 70.f || m_vBombPos.z < -70.f)
		return true;

	return false;
}

void SphereRocket::Update(const _float& dt)
{
	if (m_bDead)
		return;

	if (RobotCollisionCheck())
	{
		RobotController::GetInstance()->SetProjectileResult(m_iMasterNum, false, m_vBombPos, 7.5f, 30);
		m_bDead = true;
		return;
	}
	else if (CollisionCheck())
	{
		RobotController::GetInstance()->SetProjectileResult(m_iMasterNum, false, m_vBombPos, 7.5f, 30);
		m_bDead = true;
		return;
	}

	m_pTransform->AddPosition(m_vDir * dt * 15.f);

	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void SphereRocket::Render()
{
	CGameObject::Render();
}

void SphereRocket::Destroy()
{
	CGameObject::Destroy();
}

RESULT SphereRocket::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint masterNum, vec3 vDir)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_iMasterNum = masterNum;
	m_vDir = vDir;

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

SphereRocket* SphereRocket::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _uint masterNum, vec3 vDir)
{
	SphereRocket* pInstance = new SphereRocket();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, masterNum, vDir))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}