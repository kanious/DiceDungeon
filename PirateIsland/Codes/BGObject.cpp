#include "BGObject.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "AABB.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"
#include "UIManager.h"
#include "SoundMaster.h"
#include "LightMaster.h"

USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr), m_soundTag(""), m_vDestPos(vec3(0.f)), m_vMiddlePos(vec3(0.f)), m_vDir(vec3(0.f))
	, m_fSpeed(10.f), m_bMoving(false), m_bArriving(false), m_bMiddlePoint(false)
	, m_bLightTurnOnDelay(false), m_bLightTurnOffDelay(false), m_fDelay(1.f), m_fElapseTime(0.f)
{
	m_bDebug = false;
}

BGObject::~BGObject()
{
}

void BGObject::SetMoving(vec3 vStartPos, vec3 vDestPos, _bool mode, _bool middle, vec3 vMiddlePos)
{
	if (nullptr == m_pTransform)
		return;

	m_bMoving = true;
	m_bArriving = mode;
	if (!m_bArriving)
		m_bLightTurnOffDelay = true;
	m_bMiddlePoint = middle;
	m_vMiddlePos = vMiddlePos;
	m_vDestPos = vDestPos;

	if (middle)
		m_vDir = vMiddlePos - vStartPos;
	else
		m_vDir = vDestPos - vStartPos;
	m_vDir = normalize(m_vDir);

 	m_pTransform->SetPosition(vStartPos);

	CSoundMaster::GetInstance()->PlaySound("creaking");
}

void BGObject::Moving(const _float& dt)
{
	vec3 vPos = m_pTransform->GetPosition();
	_float fDist = 0.f;
	if (m_bMiddlePoint)
	{
		fDist = distance(m_vMiddlePos, vPos);
		if (fDist < 0.1f)
		{
			m_bMiddlePoint = false;
			m_pTransform->SetPosition(m_vMiddlePos);
			m_vDir = m_vDestPos - m_vMiddlePos;
			m_vDir = normalize(m_vDir);
		}
	}
	else
	{
		fDist = distance(m_vDestPos, vPos);
		if (fDist < 0.1f)
		{
			m_bMoving = false;
			m_pTransform->SetPosition(m_vDestPos);
			if (m_bArriving)
				m_bLightTurnOnDelay = true;
			else
				m_bEnable = false;

			CSoundMaster::GetInstance()->StopSound("creaking");
			return;
		}
	}

	m_pTransform->AddPosition(m_vDir * m_fSpeed * dt);
	RotationCheck(dt);
}

void BGObject::RotationCheck(const _float& dt)
{
	vec3 vLook = m_pTransform->GetLookVector();
	_float fAngleGap = acosf(dot(m_vDir, vLook));
	fAngleGap = degrees(fAngleGap);
	vec3 vRight = m_pTransform->GetRightVector();
	_float fDir = dot(m_vDir, vRight);
	if (fAngleGap >= 1.f)
	{
		if (0.f <= fDir)
			m_pTransform->AddRotationY(fAngleGap * dt * 3);
		else
			m_pTransform->AddRotationY(-fAngleGap * dt * 3);

		if (m_pTransform->GetRotationY() > 360.f)
			m_pTransform->AddRotationY(-360.f);
		else if (m_pTransform->GetRotationY() < 0.f)
			m_pTransform->AddRotationY(360.f);
	}
}

void BGObject::Update(const _float& dt)
{
	if (!m_bEnable)
		return;

	if (nullptr == m_pTransform)
		return;

	if (m_bMoving)
	{
		if (m_bLightTurnOffDelay)
		{
			m_fElapseTime += dt;
			if (m_fDelay <= m_fElapseTime)
			{
				m_bLightTurnOffDelay = false;
				m_fElapseTime = 0.f;
			}
		}
		else
			Moving(dt);
	}
	if (m_bLightTurnOnDelay)
	{
		m_fElapseTime += dt;
		if (m_fDelay <= m_fElapseTime)
		{
			m_bLightTurnOnDelay = false;
			m_fElapseTime = 0.f;
			CLightMaster::GetInstance()->TurnOnLight("SpotLightToShip");
		}
	}

	CGameObject::Update(dt);

	if (!strcmp(m_meshName.c_str(), "PirateShip") && nullptr != m_pTransform)
	{
		vec3 vPos = m_pTransform->GetPosition();
		vec3 vLook = m_pTransform->GetLookVector();

		vec3 vLight1 = vPos + (vLook * 1.5f);
		vLight1.y += 0.9f;
		CLightMaster::GetInstance()->SetLightPosition("ShipPointLight1", vLight1);

		vec3 vLight2 = vPos;
		vLight2.y += 0.5f;
		CLightMaster::GetInstance()->SetLightPosition("ShipPointLight2", vLight2);

		vec3 vSpotLight = vPos + (vLook * 3.f);
		vSpotLight.y += 1.f;
		CLightMaster::GetInstance()->SetLightPosition("ShipSpotLight", vSpotLight);
		vec3 vPos2 = vSpotLight + vLook;
		vPos2.y -= 1.f;
		vec3 vDir = vPos2 - vSpotLight;
		vDir = normalize(vDir);
		CLightMaster::GetInstance()->SetLightDirection("ShipSpotLight", vDir);
	}

	if (nullptr != m_pRenderer)
		m_pRenderer->AddRenderObj(this);
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
	m_soundTag = soundTag;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox_AABB = m_pMesh->GetBoundingBoxAABB();
		if (nullptr != m_pBoundingBox_AABB)
			m_pBoundingBox_AABB->SetTransform(m_pTransform);
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
