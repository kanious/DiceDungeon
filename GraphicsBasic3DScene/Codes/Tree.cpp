#include "Tree.h"
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

Tree::Tree()
	: m_pMesh(nullptr)
{
}

Tree::~Tree()
{
}

void Tree::Update(const _float& dt)
{
	CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void Tree::Render()
{
	CGameObject::Render();
}

void Tree::Destroy()
{
	CGameObject::Destroy();
}

RESULT Tree::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
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

Tree* Tree::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	Tree* pInstance = new Tree();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

CGameObject* Tree::Clone()
{
	AddRefCnt();
	return this;
}
