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
#include "LightMaster.h"
#include "Shader.h"
#include "LightMaster.h"
#include "Light.h"


USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr), m_bTransparent(false)
{
	m_bDebug = false;
}

BGObject::~BGObject()
{
}

void BGObject::Destroy()
{
	CGameObject::Destroy();
}

void BGObject::Update(const _float& dt)
{
	if (!m_bEnable)
		return;

	if (nullptr != m_pMesh)
	{
		m_pMesh->SetSelcted(m_bSelected);
		m_pMesh->SetWireFrame(m_bWireFrame);
		m_pMesh->SetTransparency(m_bTransparency);
	}

	CGameObject::Update(dt);

	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderObj(this, m_bTransparent);
}

void BGObject::Render()
{
	CGameObject::Render();
}

RESULT BGObject::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID,
	vec3 vPos, vec3 vRot, vec3 vScale, _bool transparent)
{
	SetupGameObject(sTag, lTag, oTag);
	m_objName = meshID;
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_bTransparent = transparent;

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox = m_pMesh->GetBoundingBox();
		if (nullptr != m_pBoundingBox)
			m_pBoundingBox->SetTransform(m_pTransform);
		m_pMesh->SetWireFrame(false);
		m_pMesh->SetDebugBox(false);
		m_pMesh->SetTransparency(m_bTransparent);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

BGObject* BGObject::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID,
	vec3 vPos, vec3 vRot, vec3 vScale, _bool transparent)
{
	BGObject* pInstance = new BGObject();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, transparent))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
