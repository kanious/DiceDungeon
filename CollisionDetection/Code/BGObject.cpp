#include "BGObject.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "BoundingBox.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "QuadTree.h"


USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr)
{
	m_bDebug = false;
}

BGObject::~BGObject()
{
}

TRIANGLE* BGObject::GetTrianglesFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleArray();

	return nullptr;
}

_uint BGObject::GetTriangleNumber()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleNumber();

	return 0;
}

COctree* BGObject::GetOctreeFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetOctree();

	return nullptr;
}

void BGObject::SetupOctree(_uint depth)
{
	if (nullptr != m_pMesh)
		m_pMesh->Ready_Qctree(depth);
}

void BGObject::Update(const _float& dt)
{
	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);

		if (nullptr != m_pMesh)
		{
			m_pMesh->SetWireFrame(false);
			m_pMesh->SetDebugBox(false);
		}
	}
}

void BGObject::Render()
{
	CGameObject::Render();
}

void BGObject::Destroy()
{
	CGameObject::Destroy();
}

RESULT BGObject::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
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
		m_pBoundingBox = m_pMesh->GetBoundingBox();
		if (nullptr != m_pBoundingBox)
			m_pBoundingBox->SetTransform(m_pTransform);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

BGObject* BGObject::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
{
	BGObject* pInstance = new BGObject();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, soundTag))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
