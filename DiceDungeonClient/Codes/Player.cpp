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
#include "TileMaster.h"
#include "SceneDungeon.h"
#include "UIHealthInfo.h"
#include "UIManager.h"

#include <sstream>


USING(Engine)
USING(glm)
USING(std)

Player::Player()
	: m_pMesh(nullptr), m_vVelocity(vec3(0.f)), m_vOriginPosition(vec3(0.f))
	, m_fSpeed(25.f), m_fRotSpeed(20.f), m_pAnimator(nullptr), m_vTargetPos(vec3(0.f)), m_bMoving(false)
	, m_bMotionEnd(false), m_pHealthUI(nullptr), m_bDeath(false)
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

void Player::StartMoving()
{
	if (m_bMoving)
		return;

	while(true)
	{
		if (0 == m_queuePos.size())
			break;
		m_queuePos.pop();
	}

	CTileMaster::GetInstance()->TileFill(m_iTileIdx, false);
	CTileMaster::GetInstance()->GetBestList(m_queuePos, 9999, m_iTileIdx);
	CTileMaster::GetInstance()->TileFill(m_iTileIdx, true);

	if (0 == m_queuePos.size())
		return;

	m_vTargetPos = m_queuePos.front();
	m_queuePos.pop();

	m_bMoving = true;
	if (nullptr != m_pAnimator)
		m_pAnimator->ChangeAnimation("true_pumpkin_king_run");
}

void Player::Hit()
{
	if (nullptr != m_pAnimator)
	{
		m_bMotionEnd = false;
		m_iCurHealth -= 10;
		
		if (0 < m_iCurHealth)
			m_pAnimator->ChangeAnimation("true_pumpkin_king_hurt");
		else
		{
			m_pAnimator->ChangeAnimation("true_pumpkin_king_death");
			m_bDeath = true;
			if (nullptr != m_pScene)
				m_pScene->GameOver();
		}
	}
}

void Player::Reset()
{
	m_pTransform->SetPosition(m_vOriginPos);
	m_pTransform->SetRotation(m_vOriginRot);
	m_iTileIdx = CTileMaster::GetInstance()->FindNearestTile(m_vOriginPos);
	m_iCurHealth = m_iMaxHealth;
}

void Player::ChangeAnimation(string tag)
{
	m_pAnimator->SetPause(false);
	m_pAnimator->ChangeAnimation(tag);
}

void Player::SetAnimationBlending(_bool value)
{
	m_pAnimator->SetBlendingOption(value);
}

// Check movement
void Player::MovingCheck(const _float& dt)
{
	vec3 vMyPos = m_pTransform->GetPosition();
	_float fDist = distance(vMyPos, m_vTargetPos);
	if (fDist < 0.1f)
	{
		m_pTransform->SetPosition(m_vTargetPos);

		if (0 == m_queuePos.size())
		{
			if (nullptr != m_pAnimator)
				m_pAnimator->ChangeAnimation("true_pumpkin_king_idle");
			m_bMoving = false;
			if (nullptr != m_pScene)
				m_pScene->ResetPicking();
			return;
		}
		else
		{
			m_vTargetPos = m_queuePos.front();
			m_queuePos.pop();
		}
	}

	vec3 vDir = m_vTargetPos - vMyPos;
	vDir = normalize(vDir);

	// Check Angle first
	vec3 vLook = m_pTransform->GetLookVector();
	_float fAngleGap = GetAngle(vDir, vLook);
	if (1.f < fAngleGap)
	{
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
	m_pTransform->AddPosition(vDir * dt * m_fSpeed);
}

void Player::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pAnimator)
		return;

}
 
// Basic Update Function
void Player::Update(const _float& dt)
{
	if (!m_bEnable)
		return;

	if (!m_bDeath)
	{
		if (m_bMoving)
			MovingCheck(dt);

		//KeyCheck(dt);
	}

	CGameObject::Update(dt);

	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderObj(this);
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
	SafeDestroy(m_pHealthUI);

	CGameObject::Destroy();
}

// Initialize
RESULT Player::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, SceneDungeon* pScene)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_vOriginPos = vPos;
	m_vOriginRot = vRot;

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

		m_pAnimator = Animator::Create("true_pumpkin_king_idle", false, this, pScene);
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

	m_iTileIdx = CTileMaster::GetInstance()->FindNearestTile(vPos);

	// Health UI
	m_iMaxHealth = 100;
	m_iCurHealth = 100;
	m_meshName = "Player";
	m_pHealthUI = UIHealthInfo::Create(this);
	UIManager::GetInstance()->AddHealthUI(m_pHealthUI);

	return PK_NOERROR;
}

// Create an instance
Player* Player::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, SceneDungeon* pScene)
{
	Player* pInstance = new Player();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, pScene))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
