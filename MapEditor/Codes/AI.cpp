#include "AI.h"
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
#include "Player.h"
#include "InputDevice.h"


USING(Engine)
USING(glm)
USING(std)

static _bool isActiveSeek = true;
static _bool isActiveFlee = true;
static _bool isActivePursue = true;
static _bool isActiveEvade = true;
static _bool isActiveApproach = true;

AI::AI()
	: m_pMesh(nullptr), m_pTargetPlayer(nullptr), m_bLeaving(false), m_fSpeed(0.f), m_fMaxSpeed(5.f)
	, m_fAccel(0.05f), m_vOriginPos(vec3(0.f))
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

AI::~AI()
{
}

void AI::SetAI(eAIType type, Player* pPlayer)
{
	m_eAIType = type;
	m_pTargetPlayer = pPlayer;

	if (nullptr == m_pMesh)
		return;

	switch (type)
	{
	case AI_SEEK:
		break;

	case AI_FLEE:
		m_pMesh->SetTexture("Drone_blue");
		break;

	case AI_PURSUE:
		m_pMesh->SetTexture("Drone_yellow");
		break;

	case AI_EVADE:
		m_pMesh->SetTexture("Drone_green");
		break;

	case AI_APPROACH:
		m_pMesh->SetTexture("Drone_purple");
		break;
	}
}

void AI::Seek(const _float& dt)
{
	if (nullptr == m_pTargetPlayer || !isActiveSeek)
		return;

	vec3 vTargetPos = m_pTargetPlayer->GetOriginPosition();
	vec3 vMyPos = m_pTransform->GetPosition();
	//_float fDist = distance(vTargetPos, vMyPos);
	//if (10.f < fDist)
	//{
	//	m_fSpeed = 0.f;
	//	return;
	//}

	vec3 vDir = vMyPos - vTargetPos;
	vDir = normalize(vDir);
	vec3 vLook = m_pTargetPlayer->GetLookVector();

	_float fAngleGap = GetAngle(vDir, vLook);
	if (90.f >= fAngleGap)
	{
		m_fSpeed = 0.f;
		return;
	}

	TurnAndMove(-vDir, dt);
}

void AI::Flee(const _float& dt)
{
	if (nullptr == m_pTargetPlayer || !isActiveFlee)
		return;

	vec3 vTargetPos = m_pTargetPlayer->GetOriginPosition();
	vec3 vMyPos = m_pTransform->GetPosition();
	_float fDist = distance(vTargetPos, vMyPos);

	if (m_bLeaving)
	{
		if (35.f < fDist)
		{
			m_fSpeed = 0.f;
			m_bLeaving = false;
			return;
		}
	}
	else
	{
		if (20.f < fDist)
		{
			m_fSpeed = 0.f;
			return;
		}
		else
			m_bLeaving = true;
	}

	vec3 vDir = vMyPos - vTargetPos;
	vDir = normalize(vDir);
	vec3 vLook = m_pTargetPlayer->GetLookVector();

	_float fAngleGap = GetAngle(vDir, vLook);
	if (90.f <= fAngleGap)
	{
		m_fSpeed = 0.f;
		return;
	}

	TurnAndMove(vDir, dt);
}

void AI::Pursue(const _float& dt)
{
	if (nullptr == m_pTargetPlayer || !isActivePursue)
		return;

	vec3 vTargetPos = m_pTargetPlayer->GetOriginPosition() + (m_pTargetPlayer->GetVelocity() * 2.f);
	vec3 vMyPos = m_pTransform->GetPosition();
	//_float fDist = distance(vTargetPos, vMyPos);
	//if (20.f < fDist)
	//{
	//	m_fSpeed = 0.f;
	//	return;
	//}

	vec3 vDir = vMyPos - vTargetPos;
	vDir = normalize(vDir);
	vec3 vLook = m_pTargetPlayer->GetLookVector();
	
	_float fAngleGap = GetAngle(vDir, vLook);
	if (30.f >= fAngleGap)
	{
		m_fSpeed = 0.f;
		return;
	}

	TurnAndMove(-vDir, dt);
}

void AI::Evade(const _float& dt)
{
	if (nullptr == m_pTargetPlayer || !isActiveEvade)
		return;

	vec3 vTargetPos = m_pTargetPlayer->GetPosition();
	vec3 vMyPos = m_pTransform->GetPosition();
	_float fDist = distance(vTargetPos, vMyPos);

	if (m_bLeaving)
	{
		if (fDist > 35.f)
		{
			m_fSpeed = 0.f;
			m_bLeaving = false;
			return;
		}
	}
	else
	{
		if (fDist > 20.f)
		{
			m_fSpeed = 0.f;
			return;
		}
		else
			m_bLeaving = true;
	}

	vec3 vDir = vMyPos - vTargetPos;
	vDir = normalize(vDir);
	vec3 vLook = m_pTargetPlayer->GetLookVector();

	_float fAngleGap = GetAngle(vDir, vLook);
	if (30.f <= fAngleGap)
	{
		m_fSpeed = 0.f;
		m_bLeaving = false;
		return;
	}


	TurnAndMove(vDir, dt);
}

void AI::Approach(const _float& dt)
{
	if (nullptr == m_pTargetPlayer || !isActiveApproach)
		return;

	vec3 vTargetPos = m_pTargetPlayer->GetPosition();
	vec3 vMyPos = m_pTransform->GetPosition();
	_float fDist = distance(vTargetPos, vMyPos);
	if (10.f > fDist )
	{
		m_fSpeed = 0.f;
		return;
	}

	vec3 vDir = vTargetPos - vMyPos;
	vDir = normalize(vDir);
	TurnAndMove(vDir, dt);
}

_float AI::GetAngle(vec3 vDir1, vec3 vDir2)
{
	_float fDot = dot(vDir1, vDir2);
	_float fAngleGap = safe_acos(fDot);
	return degrees(fAngleGap);
}

_float AI::safe_acos(_float value)
{
	if (value <= -1.0f)
		return radians(180.f);
	else if (value >= 1.0f)
		return 0.f;
	else
		return acos(value);
}

void AI::TurnAndMove(vec3 vDir, const _float& dt)
{
	// Rotation
	vec3 vLook = m_pTransform->GetLookVector();
	_float fAngleGap = GetAngle(vDir, vLook);
	if (1.f < fAngleGap)
	{
		vec3 vRight = m_pTransform->GetRightVector();
		_float fDot = dot(vDir, vRight);
		_float fY = m_pTransform->GetRotationY();
		if (0.0f <= fDot)
		{
			fY += fAngleGap * dt * 2.f;
			if (fY > 360.f)
				fY -= 360.f;
			m_pTransform->SetRotationY(fY);
		}
		else
		{
			fY -= fAngleGap * dt * 2.f;
			if (fY < 0.f)
				fY += 360.f;
			m_pTransform->SetRotationY(fY);
		}
	}


	// Moving
	m_fSpeed += m_fAccel;
	if (m_fSpeed > m_fMaxSpeed)
		m_fSpeed = m_fMaxSpeed;
	vec3 vPos = m_pTransform->GetPosition();
	vPos += vLook * dt * m_fSpeed;
	CheckBoundary(vPos);

	m_pTransform->SetPosition(vPos);
}

void AI::CheckBoundary(vec3& vPos)
{
	if (vPos.x > 50.f)
		vPos.x = 50.f;
	else if (vPos.x < -50.f)
		vPos.x = -50.f;

	if (vPos.z > 50.f)
		vPos.z = 50.f;
	else if (vPos.z < -50.f)
		vPos.z = -50.f;
}

void AI::KeyCheck()
{
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_R))
		m_pTransform->SetPosition(m_vOriginPos);

	static _bool isF3Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F3))
	{
		if (!isF3Down)
		{
			isF3Down = true;

			isActiveSeek = true;
			isActiveFlee = true;
			isActivePursue = true;
			isActiveEvade = true;
			isActiveApproach = true;
		}
	}
	else
		isF3Down = false;

	static _bool isF4Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F4))
	{
		if (!isF4Down)
		{
			isF4Down = true;

			isActiveSeek = false;
			isActiveFlee = false;
			isActivePursue = false;
			isActiveEvade = false;
			isActiveApproach = false;
		}
	}
	else
		isF4Down = false;

	static _bool isF5Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F5))
	{
		if (!isF5Down)
		{
			isF5Down = true;

			isActiveSeek = !isActiveSeek;
		}
	}
	else
		isF5Down = false;

	static _bool isF6Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F6))
	{
		if (!isF6Down)
		{
			isF6Down = true;

			isActiveFlee = !isActiveFlee;
		}
	}
	else
		isF6Down = false;

	static _bool isF7Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F7))
	{
		if (!isF7Down)
		{
			isF7Down = true;

			isActivePursue = !isActivePursue;
		}
	}
	else
		isF7Down = false;

	static _bool isF8Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F8))
	{
		if (!isF8Down)
		{
			isF8Down = true;

			isActiveEvade = !isActiveEvade;
		}
	}
	else
		isF8Down = false;

	static _bool isF9Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F9))
	{
		if (!isF9Down)
		{
			isF9Down = true;

			isActiveApproach = !isActiveApproach;
		}
	}
	else
		isF9Down = false;
}

void AI::Update(const _float& dt)
{
	if (m_bEnable)
	{
		KeyCheck();

		switch (m_eAIType)
		{
		case AI_SEEK:
			Seek(dt);
			break;

		case AI_FLEE:
			Flee(dt);
			break;

		case AI_PURSUE:
			Pursue(dt);
			break;

		case AI_EVADE:
			Evade(dt);
			break;

		case AI_APPROACH:
			Approach(dt);
			break;
		}

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void AI::Render()
{
	CGameObject::Render();
}

void AI::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

RESULT AI::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_vOriginPos = vPos;

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
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

AI* AI::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale)
{
	AI* pInstance = new AI();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
