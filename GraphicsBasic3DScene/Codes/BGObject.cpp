#include "BGObject.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "AABB.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"

USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr)
{
}

BGObject::~BGObject()
{
}

string BGObject::GetMeshID()
{
 	string id = "";

	if (nullptr != m_pMesh)
		id = m_pMesh->GetMeshID();

	return id;
}

void BGObject::Update(const _float& dt)
{
	CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void BGObject::Render()
{
	CGameObject::Render();
}

void BGObject::Destroy()
{
	CGameObject::Destroy();
}

RESULT BGObject::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	AttachComponent("Mesh", m_pMesh);
	m_pMesh->SetTransform(m_pTransform);
	m_pBoundingBox_AABB = m_pMesh->GetBoundingBoxAABB();

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

BGObject* BGObject::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	BGObject* pInstance = new BGObject();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

CGameObject* BGObject::Clone()
{
	AddRefCnt();
	return this;
}
