#include "Target.h"
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
#include "Function.h"
#include "Define.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"


USING(Engine)
USING(glm)
USING(std)

Target::Target()
	: m_pMesh(nullptr), m_soundTag(""), m_bHovered(false), m_fHoverElapsed(0.f), m_fHoverTime(HOVER_TIME)
	, m_fMoveElapsed(0.f), m_fMoveTime(0.f), m_vTargetDir(vec3(0.f)), m_fSpeed(0.f), m_fAccel(0.f)
	, m_fMaxSpeed(0.2f)
{
	m_bDebug = false;
}

Target::~Target()
{
}

void Target::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (nullptr == m_pTransform)
			return;

		// Moving
		m_fMoveElapsed += dt;
		if (m_fMoveElapsed >= m_fMoveTime)
		{
			GetNewLocation();
		}

		m_fSpeed += m_fAccel * dt;
		if (m_fSpeed > m_fMaxSpeed)
			m_fSpeed = m_fMaxSpeed;

		vec3 vPos = m_pTransform->GetPosition();
		vPos += m_vTargetDir * m_fSpeed;
		if (CheckBoundary(vPos))
			m_pTransform->SetPosition(vPos);
		else
		{
			GetNewLocation();
		}

		// Cursor Hover
		if (m_bHovered)
		{
			m_fHoverElapsed += dt;
			if (m_fHoverElapsed >= m_fHoverTime)
			{
				m_fHoverElapsed = 0.f;
				m_bHovered = false;
				Boom();
			}
		}
		else
			m_fHoverElapsed = 0.f;

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void Target::Render()
{
	CGameObject::Render();
}

void Target::Boom()
{
	if (nullptr == m_pTransform)
		return;

	PhysicsSystem::GetInstance()->AddKillCount();
	ParticleSystem::GetInstance()->SendEffect(m_pTransform->GetPosition());

	_float xRand = GetRandNum(-4900, 4900) / 100.f;
	_float zRand = GetRandNum(-4900, 4900) / 100.f;
	
	m_pTransform->SetPosition(vec3(xRand, SPHERE_HEIGHT, zRand));

	GetNewLocation();
}

void Target::GetNewLocation()
{
	PhysicsSystem::GetInstance()->GetRandomMovingInfo(m_fMoveTime, m_fAccel, m_vTargetDir);
	m_vTargetDir = m_vTargetDir - m_pTransform->GetPosition();
	m_vTargetDir = normalize(m_vTargetDir);
	m_fMoveElapsed = 0.f;
	m_fSpeed = 0.f;
}

_bool Target::CheckBoundary(vec3 vPos)
{
	if (vPos.x > 50.f || vPos.x < -50.f || vPos.z > 50.f || vPos.z < -50.f)
		return false;

	return true;
}

void Target::Destroy()
{
	CGameObject::Destroy();
}

RESULT Target::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;
	m_soundTag = soundTag;

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

Target* Target::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
{
	Target* pInstance = new Target();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, soundTag))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
