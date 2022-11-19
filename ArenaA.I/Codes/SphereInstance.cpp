#include "SphereInstance.h"
#include "Renderer.h"
#include "ComponentMaster.h"
#include "Layer.h"
#include "ArenaDefine.h"

#include "Mesh.h"
#include "AABB.h"
#include "Component.h"
#include "Transform.h"


USING(Engine)
USING(glm)
USING(std)

SphereInstance::SphereInstance()
	: m_pMesh(nullptr), m_bMain(false), m_fAliveTime(1.f), m_fAliveTimeElapse(0.f)
	, m_fSpeed(0.f), m_vOriginPos(vec3(0.f)), m_vTargetPos(vec3(0.f))
{
}

SphereInstance::~SphereInstance()
{
}

void SphereInstance::Update(const _float& dt)
{
	if (m_bDead)
		return;

	if (!m_bMain)
	{
		m_fAliveTimeElapse += dt;
		if (m_fAliveTimeElapse >= m_fAliveTime)
		{
			m_bDead = true;
			return;
		}
	}

	if (m_bMain)
	{
		if (nullptr == m_pTransform)
			return;

		vec3 vPos = m_pTransform->GetPosition();
		_float fDist = distance(vPos, m_vOriginPos);
		_float fDist2 = distance(m_vTargetPos, m_vOriginPos);
		if (fDist > fDist2)
		{
			MakeChild(1, vec3(0.f));
			m_bDead = true;
			return;
		}

		vec3 vDir = m_vTargetPos - vPos;
		vDir = normalize(vDir);
		m_pTransform->AddPosition(vDir * dt * m_fSpeed);

		MakeChild(m_fChildCount, vDir * dt * m_fSpeed);
	}

	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void SphereInstance::Render()
{
	CGameObject::Render();
}

void SphereInstance::Destroy()
{
	CGameObject::Destroy();
}

void SphereInstance::MakeChild(_uint num, vec3 vDir)
{
	if (nullptr == m_pLayer || nullptr == m_pTransform)
		return;

	vec3 vPos = m_pTransform->GetPosition();
	vec3 vScale = m_pTransform->GetScale();
	CGameObject* pGameObject = SphereInstance::Create(m_sceneTag,m_layerTag, m_objTag, m_pLayer,
		"sphere", vPos - vDir, vec3(0.f), vScale, false, m_vTargetPos);
	if (nullptr == pGameObject)
		return;
	m_pLayer->AddGameObject(pGameObject);

	if (1 < num)
	{
		_float fRatio = 1.f / num;
		for (int i = 0; i < num; ++i)
		{
			pGameObject = SphereInstance::Create(m_sceneTag, m_layerTag, m_objTag, m_pLayer,
				"sphere", vPos - (vDir * (fRatio * i)), vec3(0.f), vScale, false, m_vTargetPos);
			if (nullptr == pGameObject)
				return;
			m_pLayer->AddGameObject(pGameObject);
		}
	}
}

RESULT SphereInstance::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _bool isMain, vec3 targetPos)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_bMain = isMain;
	m_vTargetPos = targetPos;
	m_vOriginPos = vPos;

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

	_float fReachedTime = 0.1f;
	_float fDist = distance(vPos, m_vTargetPos);
	m_fSpeed = (fDist / fReachedTime); //0.5f : reached time
	m_fChildCount = fDist / 10.f;

	return PK_NOERROR;
}

SphereInstance* SphereInstance::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, _bool isMain, vec3 targetPos)
{
	SphereInstance* pInstance = new SphereInstance();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, isMain, targetPos))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
