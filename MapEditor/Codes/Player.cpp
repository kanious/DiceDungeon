#include "Player.h"
#include "ComponentMaster.h"
#include "Layer.h"
#include "Function.h"

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
#include "InputDevice.h"
#include "Animator.h"
#include "AnimationManager.h"

#include <sstream>


USING(Engine)
USING(glm)
USING(std)

Player::Player()
	: m_pMesh(nullptr), m_vVelocity(vec3(0.f)), m_vOriginPosition(vec3(0.f)), m_fSpeed(5.f), m_fRotSpeed(40.f)
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

Player::~Player()
{
}

// Return Mesh type
const string Player::GetMeshType()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetMeshType();

	return "";
}

// Basic Update Function
void Player::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (nullptr != m_pMesh)
		{
			m_pMesh->SetSelcted(m_bSelected);
			m_pMesh->SetWireFrame(m_bWireFrame);
			m_pMesh->SetTransparency(m_bTransparency);
		}

		CGameObject::Update(dt);
		
		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this, m_bTransparency);
	}
}

// Basic Render Function
void Player::Render()
{
	CGameObject::Render();
}

// Call instead of destructor to manage class internal data
void Player::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

// Initialize
RESULT Player::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_objName = meshID;
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

		m_pAnimator = Animator::Create(this);
		m_pMesh->SetAnimController(m_pAnimator);
		AnimationManager::GetInstance()->AddAnimator(m_pAnimator);
		m_pAnimator->SetIsPlaying(true);

		mat4x4* pmat = new mat4x4(1.f);
		*pmat = scale(*pmat, vec3(0.01f));
		m_pMesh->SetMatrix(pmat);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

// Create an instance
Player* Player::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale)
{
	Player* pInstance = new Player();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
