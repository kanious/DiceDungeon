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
	, m_pAnimator(nullptr)
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

Player::~Player()
{
}

void Player::SetTarget()
{
	AnimationManager::GetInstance()->SetTargetAnimator(this);

	if (nullptr != m_pMesh)
		m_pMesh->SetSelcted(true);
}

void Player::Deselect()
{
	if (nullptr != m_pMesh)
		m_pMesh->SetSelcted(false);
}

void Player::Update(const _float& dt)
{
	if (m_bEnable)
	{
		CGameObject::Update(dt);

		if (nullptr != m_pMesh)
			m_pMesh->SetAnimEaseType((_uint)m_eEaseType);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void Player::Render()
{
	CGameObject::Render();
}

void Player::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

RESULT Player::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, eAnimType animType, eEaseType easeType, _bool randomAnim)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_eEaseType = easeType;

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

		m_pAnimator = Animator::Create(animType, easeType, this, randomAnim);
		m_pMesh->SetAnimController(m_pAnimator);
		AnimationManager::GetInstance()->AddAnimator(m_pAnimator);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

Player* Player::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, eAnimType animType, eEaseType easeType, _bool randomAnim)
{
	Player* pInstance = new Player();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, animType, easeType, randomAnim))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
