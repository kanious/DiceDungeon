#include "Enemy.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "Layer.h"
#include "Texture.h"
#include "BattleManager.h"

USING(Engine)

Enemy::Enemy()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;

	m_vVelocity = vec3(0.f);
	m_fMass = 1.f;
	m_fInvMass = 1.f / m_fMass;
	m_fCoef = 0.9f;

	m_vForce = vec3(0.f, -9.8f, 0.f);
	m_vAcc = vec3(0.f);
	m_vDir = vec3(0.f);
	m_fPower = 0.f;

	m_iMaxHP = 200;
	m_iCurrentHP = m_iMaxHP;
}

Enemy::~Enemy()
{
}

void Enemy::Update(const _float& dt)
{
	if (nullptr != m_pTransform)
	{
		if (m_fPower <= 0.0f)
			m_fPower = 0.0f;

		m_vVelocity = vec3(m_vDir.x * m_fPower, 0, m_vDir.z * m_fPower);
		m_vForce = vec3(m_vDir.x * m_fPower, -9.8f, m_vDir.z * m_fPower);

		m_vAcc = m_vForce * m_fInvMass;
		m_vVelocity += m_vAcc * dt * 5.f;

		m_pTransform->AddPosition(m_vVelocity * dt);
		m_fPower *= m_fCoef;

		CheckWall();
	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void Enemy::Render()
{
	if (nullptr == m_pTransform || nullptr == m_pConfigManager || nullptr == m_pShader)
		return;

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

void Enemy::Destroy()
{
	Engine::CGameObject::Destroy();
}

void Enemy::LookAt(vec3 vTarget)
{
	if (nullptr == m_pTransform)
		return;

	vec3 vDir = vTarget - m_pTransform->GetPosition();
	vDir = normalize(vDir);

	vec3 vLook = m_pTransform->GetLookVector();
	vLook = normalize(vLook);

	_float fDot = dot(vDir, vLook);
	_float fGap = acosf(fDot);
	fGap = degrees(fGap);
	if (fDot <= 0.f)
		m_pTransform->AddRotation(vec3(0.f, -fGap, 0.f));
	else
		m_pTransform->AddRotation(vec3(0.f, fGap, 0.f));
	m_pTransform->Update(0);
}

vec3 Enemy::GetPosition()
{
	if (nullptr != m_pTransform)
		return m_pTransform->GetPosition();

	return vec3(0.f);
}

void Enemy::Hit(vec3 vDir, _float power)
{
	m_vDir = vDir;
	m_fPower = power; m_vVelocity = vec3(0.f);
}

void Enemy::SetDamage(_int damage)
{
	m_iCurrentHP -= damage;

	if (m_iCurrentHP <= 0)
	{
		m_iCurrentHP = 0;

		RemoveComponent((_uint)COMP_TEXTURE);
		m_pDiffTexture->Destroy();
		//Clone.Texture
		CComponent* pComponent = ComponentManager::GetInstance()->CloneTexture(TEX_TANK);
		if (nullptr != pComponent)
		{
			RESULT result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
			if (PK_NOERROR == result)
				m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
			else
				MSGBox("Enemy Texture Component Attach Failed!");
		}
	}
}

void Enemy::CheckWall()
{
	vec3 vPos = m_pTransform->GetPosition();
	if (vPos.y <= 0)
		m_pTransform->SetPositionY(0.f);

	if (vPos.x >= 20.f)
	{
		m_pTransform->SetPositionX(19.f);
		vec3 newDir = GetReflectVector(m_vDir, vec3(-1.0f, 0.f, 0.f));
		newDir = normalize(newDir);
		m_vDir = newDir;
	}
	else if (vPos.x <= -20.f)
	{
		m_pTransform->SetPositionX(-19.f);
		vec3 newDir = GetReflectVector(m_vDir, vec3(1.0f, 0.f, 0.f));
		newDir = normalize(newDir);
		m_vDir = newDir;
	}

	if (vPos.z >= 20.f)
	{
		m_pTransform->SetPositionZ(19.f);
		vec3 newDir = GetReflectVector(m_vDir, vec3(0.0f, 0.f, 1.f));
		newDir = normalize(newDir);
		m_vDir = newDir;
	}
	else if (vPos.z <= -20.f)
	{
		m_pTransform->SetPositionZ(-19.f);
		vec3 newDir = GetReflectVector(m_vDir, vec3(0.0f, 0.f, -1.f));
		newDir = normalize(newDir);
		m_vDir = newDir;
	}
}

vec3 Enemy::GetReflectVector(vec3 vVector, vec3 vNormal)
{
	// R = P + 2N(-P * N)
	vec3 reflectVector = vVector + ((vNormal * 2.f) * ((vVector * -1.f) * vNormal));
	return reflectVector;
}

RESULT Enemy::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_pLayer = pParent;

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
			m_pTransform->SetPosRotScale(vPos, vRot, vScale);
			m_pTransform->Update(0);
		}
		else
			MSGBox("Enemy Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = pManager->CloneMesh(MESH_TANK);
	//pComponent = CMesh::Create("", vec3(1.f, 0.f, 0.f));
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("Enemy Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_MESH);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("Enemy Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_TANK_RED);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("Enemy Texture Component Attach Failed!");
	}

	return PK_NOERROR;
}

Enemy* Enemy::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
{
	Enemy* pInstance = new Enemy;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, pParent, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}