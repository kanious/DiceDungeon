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
	: m_pMesh(nullptr), m_vVelocity(vec3(0.f)), m_vOriginPosition(vec3(0.f))
	, m_fSpeed(15.f), m_fRotSpeed(10.f), m_pAnimator(nullptr), m_vTargetPos(vec3(0.f)), m_bMoving(false)
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

Player::~Player()
{
}

// Notify mesh instance of targeting status
void Player::SetTarget(_bool value)
{
	if (nullptr != m_pMesh)
		m_pMesh->SetSelcted(value);
}

// Set the position to move
void Player::SetTargetPos(vec3 vPos)
{
	m_vTargetPos = vPos;
	m_bMoving = true;
}

// Check movement
void Player::MovingCheck(const _float& dt)
{
	vec3 vMyPos = m_pTransform->GetPosition();
	
	_float fDist = distance(vMyPos, m_vTargetPos);
	if (fDist < 0.1f)
	{
		m_pTransform->SetPosition(m_vTargetPos);
		m_bMoving = false;
		m_pAnimator->SetIsPlaying(false);
		m_pAnimator->ResetAnimation();
		return;
	}

	vec3 vDir = m_vTargetPos - vMyPos;
	vDir = normalize(vDir);

	// Check Angle first
	vec3 vLook = m_pTransform->GetLookVector();
	_float fAngleGap = GetAngle(vDir, vLook);
	if (1.f < fAngleGap)
	{
		m_pAnimator->SetIsPlaying(false);
		m_pAnimator->ResetAnimation();

		vec3 vRight = m_pTransform->GetRightVector();
		_float fDot = dot(vDir, vRight);
		_float fY = m_pTransform->GetRotationY();
		if (0.0f <= fDot)
		{
			fY += fAngleGap * dt * m_fRotSpeed;
			if (fY > 360.f)
				fY -= 360.f;
			m_pTransform->SetRotationY(fY);
		}
		else
		{ 
			fY -= fAngleGap * dt * m_fRotSpeed;
			if (fY < 0.f)
				fY += 360.f;
			m_pTransform->SetRotationY(fY);
		}
	}
	else
	{
		m_pAnimator->SetIsPlaying(true);
		m_pTransform->AddPosition(vDir * dt * m_fSpeed);
	}
}

void Player::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice)
		return;

	static _bool isF3Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F3))
	{
		if (!isF3Down)
		{
			isF3Down = true;
			if (nullptr != m_pAnimator)
				m_pAnimator->SetBlendingOption(!m_pAnimator->GetBlendingOption());
		}
	}
	else
		isF3Down = false;

	if (m_bMoving)
		return;

	_bool pressed = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_1))
	{
		m_pAnimator->ChangeAnimation("walk");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_2))
	{
		m_pAnimator->ChangeAnimation("run");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_3))
	{
		m_pAnimator->ChangeAnimation("attack1");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_4))
	{
		m_pAnimator->ChangeAnimation("attack2");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_5))
	{
		m_pAnimator->ChangeAnimation("grab");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_6))
	{
		m_pAnimator->ChangeAnimation("pick_up");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_7))
	{
		m_pAnimator->ChangeAnimation("hurt");
		pressed = true;
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_8))
	{
		m_pAnimator->ChangeAnimation("death");
		pressed = true;
	}

	if (!pressed)
		m_pAnimator->ChangeAnimation("idle");
}
 
// Basic Update Function
void Player::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (m_bMoving)
			MovingCheck(dt);

		KeyCheck(dt);

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
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

		mat4x4* pmat = new mat4x4(1.f);
		*pmat = scale(*pmat, vec3(0.01f));
		m_pMesh->SetMatrix(pmat);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	m_pAnimator->SetIsPlaying(true);

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
