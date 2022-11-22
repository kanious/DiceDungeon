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


USING(Engine)
USING(glm)
USING(std)

BGObject::BGObject()
	: m_pMesh(nullptr), m_soundTag(""), m_bHelicopter(false), m_bFloating(false), m_bUpside(true)
	, m_fOriginY(0.f), m_fSpeed(10.f), m_fWaitingTime(3.f), m_fElapseTime(0.f)
	, m_pSphere(nullptr)
{
	m_bDebug = false;
}

BGObject::~BGObject()
{
}

TRIANGLE* BGObject::GetTrianglesFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleArray();

	return nullptr;
}

_uint BGObject::GetTriangleNumber()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleNumber();

	return 0;
}

COctree* BGObject::GetOctreeFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetOctree();

	return nullptr;
}

void BGObject::SetupOctree(_uint depth)
{
	if (nullptr != m_pMesh)
		m_pMesh->Ready_Qctree(depth);
}

void BGObject::SetDebugSphere(_bool value)
{
	if (nullptr != m_pSphere)
		m_pSphere->SetEnable(value);
}

void BGObject::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (m_bHelicopter)
		{
			if (m_bFloating)
			{
				if (m_bUpside)
				{
					m_pTransform->AddPositionY(dt * m_fSpeed);
					if (m_pTransform->GetPositionY() > m_fOriginY + 120.f)
					{
						m_pTransform->SetPositionY(m_fOriginY + 120.f);
						m_bUpside = false;
						m_bFloating = false;
					}
				}
				else
				{
					m_pTransform->AddPositionY(-dt * m_fSpeed);
					if (m_pTransform->GetPositionY() <= m_fOriginY)
					{
						m_pTransform->SetPositionY(m_fOriginY);
						m_bUpside = true;
						m_bFloating = false;
						CSoundMaster::GetInstance()->StopSound(m_soundTag);
					}
				}

				// Light
				vec3 vPos = m_pTransform->GetPosition();
				vec3 vLightPos = vPos;
				vLightPos.y += 18.f;
				CLightMaster::GetInstance()->SetLightPosition("HeliLight", vLightPos);
				
				// Sound
				CSoundMaster::GetInstance()->Set3DSoundPosition(m_soundTag, vPos.x, vPos.y, vPos.z);
			}
			else
			{
				m_fElapseTime += dt;
				if (m_fElapseTime >= m_fWaitingTime)
				{
					m_bFloating = true;
					vec3 vPos = m_pTransform->GetPosition();
					CSoundMaster::GetInstance()->Play3DSound(m_soundTag, vPos.x, vPos.y, vPos.z);
					m_fElapseTime = 0.f;
				}
			}
		}

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);

		if (nullptr != m_pMesh)
		{
			m_pMesh->SetWireFrame(false);
			m_pMesh->SetDebugBox(false);
		}
	}
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


	if ("" != soundTag)
	{
		if ("Helicopter" == soundTag)
		{
			m_bHelicopter = true;
			m_fOriginY = vPos.y;
		}
		else
			CSoundMaster::GetInstance()->Play3DSound(soundTag, vPos.x, vPos.y, vPos.z);

		m_pSphere = CloneComponent<CMesh*>("Sphere");
		if (nullptr != m_pSphere)
		{
			AttachComponent("DebugSphere", m_pSphere);
			m_pSphere->SetTransform(m_pTransform);
			m_pSphere->SetWireFrame(true);
			CShader* shader = m_pSphere->GetShader();
			if (nullptr != shader)
			{
				shader->SetColor(vec3(1.f, 1.f, 0.f));
			}
			m_pSphere->SetEnable(false);
		}
	}

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox = m_pMesh->GetBoundingBox();
		if (nullptr != m_pBoundingBox)
			m_pBoundingBox->SetTransform(m_pTransform);
		if ("FireHydrantRed" == m_meshName)
			m_pMesh->SetEnable(false);
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
