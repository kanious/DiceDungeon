#include "Bomb.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "BattleManager.h"
#include "Renderer.h"
#include "Layer.h"
#include "Texture.h"

USING(Engine)

Bomb::Bomb()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;

	m_vVelocity = vec3(0.f, 10.f, 0.f);
	m_fMass = 1.f;
	m_fInvMass = 1.f / m_fMass;
	m_fExplosionRadius = 4.f;
	m_fExplosionPower = 2.f;

	m_vForce = vec3(0.f);
	m_vAcc = vec3(0.f);
	m_vLook = vec3(0.f);

	m_fTimeElapsed = 0.f;
	m_fRandTime = 0.f;
	m_bDelay = false;
}

Bomb::~Bomb()
{
}

void Bomb::Update(const _float& dt)
{
	if (!m_bDelay)
	{
		m_fTimeElapsed += dt;
		if (m_fTimeElapsed >= m_fRandTime)
		{
			m_bDelay = true;
			m_fTimeElapsed = 0.f;
		}
		else
			return;
	}

	if (nullptr != m_pTransform)
	{
		if (m_pTransform->GetPosition().y <= 0.f)
		{
			Boom();
			return;
		}

 		m_vAcc = -m_vForce * m_fInvMass;
		m_vVelocity += m_vAcc * dt * 10.f;

 		m_pTransform->AddPosition(vec3(m_vLook.x * m_vVelocity.x * dt
			, m_vLook.y * m_vVelocity.y * dt
			, m_vLook.z * m_vVelocity.z * dt));
	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void Bomb::Render()
{
	mat4x4 matWorld = *m_pTransform->GetWorldMatrix();
	const mat4x4 matView = m_pConfigManager->GetViewMatrix();
	const mat4x4 matProj = m_pConfigManager->GetProjMatrix();
	m_pShader->SetMatrixInfo(matWorld, matView, matProj);
	m_pShader->SetTextureInfo();

	if (nullptr != m_pDiffTexture)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_pDiffTexture->GetTextureID());
	}

	Engine::CGameObject::Render();
}

void Bomb::Destroy()
{
	BattleManager::GetInstance()->SubtractBulletNum();
	Engine::CGameObject::Destroy();
}

void Bomb::SetForce(vec3 vForce)
{
	m_vForce = vForce;

	_int randNum = rand() % 201;
	m_vForce.y -= randNum * 0.01f;
}

void Bomb::Boom()
{
	if (nullptr != m_pTransform)
	{
		BattleManager::GetInstance()->BoomLocation(m_pTransform->GetPosition()
			, m_fExplosionRadius, m_fExplosionPower);
	}
	SetDead(true);
}

RESULT Bomb::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, vec3 vLook)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_vLook = vLook;
	_int randNum = rand() % 51;
	m_fRandTime = randNum * 0.01;

	ComponentManager* pManager = ComponentManager::GetInstance();
	RESULT result = PK_NOERROR;
	CComponent* pComponent = nullptr;

	//Clone.Transform
	pComponent = pManager->CloneTransform();
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TRANSFORM, pComponent);
		if (PK_NOERROR == result)
		{
			m_pTransform = dynamic_cast<CTransform*>(pComponent);

			vPos.y += 0.5f;
			m_pTransform->SetPosRotScale(vPos, vRot, vScale);
			m_pTransform->Update(0);
		}
		else
			MSGBox("SpiralBullet Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = pManager->CloneMesh(MESH_SPHERE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("SpiralBullet Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_BASE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("SpiralBullet Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_BULLET);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("SpiralBullet Texture Component Attach Failed!");
	}

	return PK_NOERROR;
}

Bomb* Bomb::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, vec3 vLook)
{
	Bomb* pInstance = new Bomb;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, vLook))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}
	else
		BattleManager::GetInstance()->AddBulletNum();

	return pInstance;
}