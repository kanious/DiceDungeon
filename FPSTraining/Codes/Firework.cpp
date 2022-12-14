#include "Firework.h"
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
#include "ParticleSystem.h"


USING(Engine)
USING(glm)
USING(std)

Firework::Firework()
	: m_pMesh(nullptr), m_vDir(vec3(0.f)), m_vVelocity(vec3(0.f)), m_vForce(vec3(0.f, -0.49f, 0.f))
	, m_fSpeed(0.f), m_fAccel(0.f), m_fAgeTime(0.f), m_fElaspedTime(0.f)
{
	m_bDebug = false;
}

Firework::~Firework()
{
}

void Firework::Update(const _float& dt)
{
	if (m_bEnable)
	{
		if (nullptr == m_pTransform)
			return;

		m_fElaspedTime += dt;
		if (m_fElaspedTime >= m_fAgeTime)
		{
			ParticleSystem::GetInstance()->ReturnEffect();
			m_bEnable = false;
			return;
		}

		m_fSpeed -= m_fAccel * dt;
		if (m_fSpeed < 0.f)
			m_fSpeed = 0.f;

		m_vVelocity += m_vDir * m_fSpeed;
		//m_vVelocity += m_vForce * dt * 10.f;
		m_vVelocity += m_vForce * m_fElaspedTime * m_fElaspedTime / 2.f;
		m_pTransform->AddPosition(m_vVelocity);

		m_vVelocity = vec3(0.f);

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);
	}
}

void Firework::Render()
{
	CGameObject::Render();
}

void Firework::SendEffect(vec3 vPos, vec3 vDir, _float speed, _float accel, _float age)
{
	if (nullptr != m_pTransform)
		m_pTransform->SetPosition(vPos);

	m_vDir = vDir;
	m_fSpeed = speed;
	m_fAccel = accel;
	m_fAgeTime = age;
	m_fElaspedTime = 0.f;

	m_bEnable = true;
}

void Firework::Destroy()
{
	CGameObject::Destroy();
}

RESULT Firework::Ready(string meshID, vec3 vPos, vec3 vScale)
{
	m_meshName = meshID;
	m_bEnable = false;

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
		m_pTransform->SetPosRotScale(vPos, vec3(0.f), vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

Firework* Firework::Create(string meshID, vec3 vPos, vec3 vScale)
{
	Firework* pInstance = new Firework();
	if (PK_NOERROR != pInstance->Ready(meshID, vPos, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
