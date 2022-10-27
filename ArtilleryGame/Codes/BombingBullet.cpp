#include "BombingBullet.h"
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
#include "Bomb.h"

USING(Engine)

BombingBullet::BombingBullet()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;

	m_vVelocity = vec3(20.f);
	m_fMass = 1.f;
	m_fInvMass = 1.f / m_fMass;
	m_fExplosionRadius = 4.f;
	m_fExplosionPower = 3.f;

	m_vForce = vec3(0.f);
	m_vAcc = vec3(0.f);
	m_fTimeElapsed = 0.f;
	m_fAngle = 0.f;
	m_fTimeBomb = 2.f;
	m_bChangeMode = false;

	m_fRandTime = 0.1f;
	m_fTimeElapsed2 = 0.f;
}

BombingBullet::~BombingBullet()
{
}

void BombingBullet::Update(const _float& dt)
{
	if (nullptr != m_pTransform)
	{
		if (m_pTransform->GetPosition().y <= 0.f)
		{
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

		if (m_bChangeMode)
		{
			if (m_fTimeElapsed >= m_fTimeBomb)
			{
				Boom();
				return;
			}
			if (m_fTimeElapsed <= 1.f && m_fTimeElapsed2 >= m_fRandTime)
			{
				ShootChildBullet();
				ShootChildBullet();
				ShootChildBullet();
				m_fTimeElapsed2 = 0;
				m_fRandTime = (rand() % 11 + 10) * 0.01f;
			}
			else
				m_fTimeElapsed2 += dt;

			m_pTransform->AddPosition(vec3(vLook.x * xDistance, 0, vLook.z * zDistance));
		}
		else
		{
			if (yDistance <= 0)
			{
				m_bChangeMode = true;
				m_fTimeElapsed = 0.f;
			}
			else
			{
				m_pTransform->AddPosition(vec3(vLook.x * xDistance, yDistance, vLook.z * zDistance));
			}
		}

	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void BombingBullet::Render()
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

void BombingBullet::Destroy()
{
	BattleManager::GetInstance()->SubtractBulletNum();
	Engine::CGameObject::Destroy();
}

void BombingBullet::SetForce(vec3 vForce)
{
	m_vForce = vForce;
}

void BombingBullet::ShootChildBullet()
{
	if (nullptr == m_pTransform)
		return;

	vec3 vPos = m_pTransform->GetPosition();
	vec3 vRot = vec3(0.f);
	vec3 vScale = vec3(0.25f);
	vec3 vLook = vec3(0.f);

	_int ranNum = rand() % 101 - 50;
	vPos.x += ranNum * 0.01f; //-0.5 ~ 0.5
	ranNum = rand() % 101 - 50;
	vPos.z += ranNum * 0.01;

	vec3 vDestPos = vec3(vPos.x, 0.f, vPos.z);
	vLook = vDestPos - vPos;
	vLook = normalize(vLook);

	CGameObject* pGameObject = Bomb::Create((eSCENETAG)m_sceneTag,
		LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, vLook);
	if (nullptr != pGameObject)
	{
		m_pLayer->AddGameObject(pGameObject);
		pGameObject->SetParentLayer(m_pLayer);
		Bomb* pBullet = dynamic_cast<Bomb*>(pGameObject);
		pBullet->SetForce(m_vForce);
	}
}

void BombingBullet::Boom()
{
	if (nullptr != m_pTransform)
	{
		BattleManager::GetInstance()->BoomLocation(m_pTransform->GetPosition()
			, m_fExplosionRadius, m_fExplosionPower);
	}
	SetDead(true);
}

RESULT BombingBullet::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
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

BombingBullet* BombingBullet::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
{
	BombingBullet* pInstance = new BombingBullet;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, angle))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}
	else
		BattleManager::GetInstance()->AddBulletNum();

	return pInstance;
}