#include "BounceBullet.h"
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

BounceBullet::BounceBullet()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;

	m_vVelocity = vec3(30.f);
	m_fMass = 7.f;
	m_fInvMass = 1.f / m_fMass;
	m_fCoef = 0.7f;
	m_fExplosionRadius = 3.f;
	m_fExplosionPower = 30.f;

	m_vForce = vec3(0.f);
	m_vAcc = vec3(0.f);
	m_fTimeElapsed = 0.f;
	m_fAngle = 0.f;
	m_iBounceCount = 0;
}

BounceBullet::~BounceBullet()
{
}

void BounceBullet::Update(const _float& dt)
{
	if (nullptr != m_pTransform)
	{
		if (m_iBounceCount > 2)
		{
			Boom();
			return;
		}
		if (m_pTransform->GetPosition().y <= 0.f)
		{
			m_pTransform->SetPositionY(0.f);
			m_fTimeElapsed = 0;
			m_vVelocity *= m_fCoef;
			m_iBounceCount++;
		}
		m_fTimeElapsed += dt;
		vec3 vLook = m_pTransform->GetLookVector();
		vLook = normalize(vLook);

		m_vAcc = m_vForce * m_fInvMass;			// acc = force * invMass
		m_vVelocity += m_vAcc * dt * 10.f;		// add acc to velocity

		// calculate the moved distance of the each direction
		_float xDistance = cos(m_fAngle) * m_vVelocity.x * dt;
		_float yDistance = sin(m_fAngle) * m_vVelocity.y * dt + (m_vForce.y * m_fMass * m_fTimeElapsed * m_fTimeElapsed / 2.f);
		_float zDistance = cos(m_fAngle) * m_vVelocity.z * dt;

		m_pTransform->AddPosition(vec3(vLook.x * xDistance, yDistance, vLook.z * zDistance));
	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void BounceBullet::Render()
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

void BounceBullet::Destroy()
{
	BattleManager::GetInstance()->SubtractBulletNum();
	Engine::CGameObject::Destroy();
}
void BounceBullet::SetForce(vec3 vForce)
{
	m_vForce = vForce;
}
void BounceBullet::Boom()
{
	if (nullptr != m_pTransform)
	{
		BattleManager::GetInstance()->BoomLocation(m_pTransform->GetPosition()
			, m_fExplosionRadius, m_fExplosionPower);
	}
	SetDead(true);
}

RESULT BounceBullet::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_fAngle = angle;

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
			MSGBox("BounceBullet Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = pManager->CloneMesh(MESH_SPHERE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("BounceBullet Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_BASE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("BounceBullet Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_BULLET);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("BounceBullet Texture Component Attach Failed!");
	}

	return PK_NOERROR;
}

BounceBullet* BounceBullet::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
{
	BounceBullet* pInstance = new BounceBullet;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, angle))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}
	else
		BattleManager::GetInstance()->AddBulletNum();

	return pInstance;
}