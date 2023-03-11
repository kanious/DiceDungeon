#include "PhysicsObject.h"
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
#include "PhysicsDefines.h"


USING(Engine)
USING(glm)
USING(std)

PhysicsObject::PhysicsObject()
	: m_pMesh(nullptr), m_pRigidBody(nullptr)
{
	m_bDebug = false;
}

PhysicsObject::~PhysicsObject()
{
}

// Basic Update Function
void PhysicsObject::Update(const _float& dt)
{
	if (nullptr == m_pRigidBody || nullptr == m_pTransform)
		return;

	m_bEnable = m_pRigidBody->GetEnable();
	if (!m_bEnable)
		return;

	CGameObject::Update(dt);
	
	m_pTransform->SetWorldMatrix(m_pRigidBody->GetModelMatrix());
	
	if (nullptr != m_pMesh)
		m_pMesh->SetPriority(true);

	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderObj(this);
}

// Basic Render Function
void PhysicsObject::Render()
{
	CGameObject::Render();
}

// Call instead of destructor to manage class internal data
void PhysicsObject::Destroy()
{
	CGameObject::Destroy();
}

// Initialize
RESULT PhysicsObject::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;

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
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		quat qRot = quat(vec3(radians(vRot.x), radians(vRot.y), radians(vRot.z)));
		m_pTransform->SetQuaternion(qRot);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

// Create an instance
PhysicsObject* PhysicsObject::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	PhysicsObject* pInstance = new PhysicsObject();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
