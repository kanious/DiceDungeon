#include "Enemy.h"
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
#include "StateMachine.h"
#include "EnemyManager.h"
#include "UIHealthInfo.h"
#include "UIManager.h"

#include <sstream>


USING(Engine)
USING(glm)
USING(std)

Enemy::Enemy()
	: m_pMesh(nullptr), m_vVelocity(vec3(0.f)), m_vOriginPosition(vec3(0.f))
	, m_fSpeed(25.f), m_fRotSpeed(20.f), m_pAnimator(nullptr), m_vTargetPos(vec3(0.f)), m_bMoving(false)
	, m_bMotion(false), m_idle(""), m_run(""), m_attack(""), m_death(""), m_pHealthUI(nullptr)
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

Enemy::~Enemy()
{
}

// Notify mesh instance of targeting status
void Enemy::SetTarget(_bool value)
{
	if (nullptr != m_pMesh)
		m_pMesh->SetSelcted(value);
}

void Enemy::StartMoving()
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
	CTileMaster::GetInstance()->GetBestList(m_queuePos, 5, m_iTileIdx);
	CTileMaster::GetInstance()->TileFill(m_iTileIdx, true);

	if (0 == m_queuePos.size())
	{
		EndTurn();
		return;
	}

	m_vTargetPos = m_queuePos.front();
	m_queuePos.pop();

	m_bMoving = true;
	if (nullptr != m_pAnimator)
		m_pAnimator->ChangeAnimation(m_run);
}

void Enemy::EndTurn()
{
	if (nullptr != m_pAnimator)
		m_pAnimator->ChangeAnimation(m_idle);
	m_bMoving = false;
	m_bMotion = false;
	m_bMotionEnd = false;
	EnemyManager::GetInstance()->SetFinishMyTurn();
}

void Enemy::RunAI()
{
	m_eAIState = STATE_IDLE;

	vec3 vPlayerPos(0.f);
	if (m_pScene->GetPlayerPos(vPlayerPos))
	{
		vec3 vMyPos = m_pTransform->GetPosition();
		m_eAIState = StateMachine::GetState(vMyPos, vPlayerPos, m_eEnemyType);
	}

	switch (m_eAIState)
	{
	case STATE_IDLE:
		Idle();
		break;

	case STATE_ATTACK:
		m_bRotate = true;
		AttackPlayer();
		break;

	case STATE_CHASE:
		ChasePlayer();
		break;
	}
}

void Enemy::Reset()
{
	m_pTransform->SetPosition(m_vOriginPos);
	m_pTransform->SetRotation(m_vOriginRot);
	m_iTileIdx = CTileMaster::GetInstance()->FindNearestTile(m_vOriginPos);
	m_iCurHealth = m_iMaxHealth;
}

void Enemy::Idle()
{
	if (nullptr != m_pAnimator)
		m_pAnimator->ChangeAnimation(m_idle);
	EndTurn();
}

void Enemy::ChasePlayer()
{
	_int playerIdx = -1;
	if (m_pScene->GetPlayerTileIdx(playerIdx))
	{
		CTileMaster::GetInstance()->PathfindingStart(m_iTileIdx, playerIdx);
		StartMoving();
	}
}

void Enemy::SeePlayer(const _float& dt)
{
	vec3 vMyPos = m_pTransform->GetPosition();
	vec3 vDir = m_vTargetPos - vMyPos;
	vDir = normalize(vDir);

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
	else
	{
		m_bRotate = false;
		
		AttackPlayer();
	}
}

void Enemy::AttackPlayer()
{
	if (m_bRotate)
	{
		m_pScene->GetPlayerPos(m_vTargetPos);
		return;
	}

	if (nullptr == m_pAnimator || nullptr == m_pScene)
	{
		EndTurn();
		return;
	}

	m_pAnimator->ChangeAnimation(m_attack);
	m_bMotion = true;
}

// Check movement
void Enemy::MovingCheck(const _float& dt)
{
	vec3 vMyPos = m_pTransform->GetPosition();
	_float fDist = distance(vMyPos, m_vTargetPos);
	if (fDist < 0.1f)
	{
		m_pTransform->SetPosition(m_vTargetPos);

		if (0 == m_queuePos.size())
		{
			EndTurn();
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

// Basic Update Function
void Enemy::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (m_bMoving)
			MovingCheck(dt);

		if (m_bRotate)
			SeePlayer(dt);

		if (m_bMotion)
		{
			if (m_bMotionEnd)
				EndTurn();
		}

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

// Basic Render Function
void Enemy::Render()
{
	CGameObject::Render();
}

// Call instead of destructor to manage class internal data
void Enemy::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pHealthUI);

	CGameObject::Destroy();
}

// Initialize
RESULT Enemy::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, SceneDungeon* pScene)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_vOriginPos = vPos;
	m_vOriginRot = vRot;

	if (meshID == "true_dummy")
		m_eEnemyType = ENEMY_DUMMY;
	else if (meshID == "true_scarecrow")
		m_eEnemyType = ENEMY_SCARECROW;
	else if (meshID == "true_crawler")
		m_eEnemyType = ENEMY_CRAWLER;
	else if (meshID == "true_clown")
		m_eEnemyType = ENEMY_CLOWN;

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

		switch (m_eEnemyType)
		{
		case ENEMY_DUMMY:
		{
			_int randNum = GetRandNum(0, 99);
			if (randNum < 30)
				m_pMesh->SetTexture("true_dummy01_Tex");
			else if (randNum > 70)
				m_pMesh->SetTexture("true_dummy02_Tex");

			m_idle = "true_dummy_idle";
			m_run = "true_dummy_run";
			m_attack = "true_dummy_attack1";
			m_death = "true_dummy_death";

			randNum = GetRandNum(50, 70);
			m_iMaxHealth = randNum;
			m_iCurHealth = randNum;
		}
			break;

		case ENEMY_CRAWLER:
			m_idle = "true_crawler_Idle";
			m_run = "true_crawler_crawl_fast";
			m_attack = "true_crawler_attack";
			m_death = "true_crawler_pounce";

			m_iMaxHealth = 250;
			m_iCurHealth = 250;
			break;
		}

		{
			m_pAnimator = Animator::Create(m_idle, true, this, pScene);
			m_pMesh->SetAnimController(m_pAnimator);
			AnimationManager::GetInstance()->AddAnimator(m_pAnimator);
			m_pAnimator->SetIsPlaying(true);

			mat4x4* pmat = new mat4x4(1.f);
			*pmat = scale(*pmat, vec3(0.01f));
			*pmat = rotate(*pmat, radians(-90.f), vec3(0.f, 1.f, 0.f));
			m_pMesh->SetMatrix(pmat);
		}
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	m_iTileIdx = CTileMaster::GetInstance()->FindNearestTile(vPos);

	// Health UI
	m_pHealthUI = UIHealthInfo::Create(this);
	UIManager::GetInstance()->AddHealthUI(m_pHealthUI);

	return PK_NOERROR;
}

// Create an instance
Enemy* Enemy::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos
	, vec3 vRot, vec3 vScale, SceneDungeon* pScene)
{
	Enemy* pInstance = new Enemy();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, pScene))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
