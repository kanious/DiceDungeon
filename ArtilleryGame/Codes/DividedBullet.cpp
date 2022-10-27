#include "DividedBullet.h"
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

DividedBullet::DividedBullet()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;

	m_vVelocity = vec3(25.f);
	m_fMass = 2.f;
	m_fInvMass = 1.f / m_fMass;
	m_fCoef = 0.8f;
	m_fExplosionRadius = 2.f;
	m_fExplosionPower = 20.f;

	m_vForce = vec3(0.f);
	m_vAcc = vec3(0.f);
	m_fTimeElapsed = 0.f;
	m_fAngle = 0.f;
	m_bParent = false;

	m_fRandTime = 0.f;
	m_bDelay = false;
}

DividedBullet::~DividedBullet()
{
}

void DividedBullet::Update(const _float& dt)
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
			if (m_bParent)
			{
				ShootChildBullet(0);
				ShootChildBullet(1);
				ShootChildBullet(2);
				ShootChildBullet(3);
				ShootChildBullet(4);
				ShootChildBullet(5);
			}
			Boom();
			return;
		}
		m_fTimeElapsed += dt;
		vec3 vLook = m_pTransform->GetLookVector();
		vLook = normalize(vLook);

		m_vAcc = m_vForce * m_fInvMass;
		m_vVelocity += m_vAcc * dt * 10.f;

		// calculate the moved distance of the each direction
		_float xDistance = cos(m_fAngle) * m_vVelocity.x * dt;
		_float yDistance = sin(m_fAngle) * m_vVelocity.y * dt + (m_vForce.y * m_fMass * m_fTimeElapsed * m_fTimeElapsed / 2.f);
		_float zDistance = cos(m_fAngle) * m_vVelocity.z * dt;

		m_pTransform->AddPosition(vec3(vLook.x * xDistance, yDistance, vLook.z * zDistance));
	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void DividedBullet::Render()
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

void DividedBullet::Destroy()
{
	BattleManager::GetInstance()->SubtractBulletNum();
	Engine::CGameObject::Destroy();
}

void DividedBullet::SetChildStatus(_float fMass, vec3 vVelocity)
{
	m_fMass = fMass;
	m_vVelocity = vVelocity;
}

void DividedBullet::SetForce(vec3 vForce)
{
	m_vForce = vForce;
}

void DividedBullet::SetExplosionPower(_float fPower)
{
	m_fExplosionPower = fPower;
}

void DividedBullet::ShootChildBullet(_uint iNum)
{
	if (nullptr == m_pTransform)
		return;

	int randNum = rand() % 61 + (60 * iNum);
	vec3 vPos = m_pTransform->GetPosition();
	vec3 vRot = vec3(0.f, m_pTransform->GetRotation().y + randNum, 0.f);
	vec3 vScale = vec3(0.25f);
	if (vRot.y > 360)
		vRot.y -= 360;
	else if (vRot.y < 0)
		vRot.y += 360;

	CGameObject* pGameObject = DividedBullet::Create((eSCENETAG)m_sceneTag,
		LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, m_fAngle * m_fCoef, false);
	if (nullptr != pGameObject)
	{
		m_pLayer->AddGameObject(pGameObject);
		pGameObject->SetParentLayer(m_pLayer);
		DividedBullet* pBullet = dynamic_cast<DividedBullet*>(pGameObject);
		pBullet->SetChildStatus(10.f, vec3(10.f));
		pBullet->SetForce(m_vForce);
		pBullet->SetExplosionPower(5.f);
	}
}

void DividedBullet::Boom()
{
	if (nullptr != m_pTransform)
	{
		BattleManager::GetInstance()->BoomLocation(m_pTransform->GetPosition()
			, m_fExplosionRadius, m_fExplosionPower);
	}
	SetDead(true);
}

RESULT DividedBullet::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle, _bool bParent)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_fAngle = angle;
	m_bParent = bParent;
	if (!m_bParent)
	{
		m_fRandTime = rand() % 51 * 0.01;
	}

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

DividedBullet* DividedBullet::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle, _bool bParent)
{
	DividedBullet* pInstance = new DividedBullet;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, angle, bParent))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}
	else
		BattleManager::GetInstance()->AddBulletNum();

	return pInstance;
}