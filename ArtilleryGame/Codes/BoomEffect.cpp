#include "BoomEffect.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "Texture.h"


USING(Engine)

BoomEffect::BoomEffect()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;

	m_fRadius = 0.f;
}

BoomEffect::~BoomEffect()
{
}

void BoomEffect::Update(const _float& dt)
{
	if (m_bDead)
		return;

	if (nullptr != m_pTransform)
	{
		vec3 vScale = m_pTransform->GetScale();
		vScale += dt * 10.f;
		if (vScale.x >= m_fRadius)
		{
			SetDead(true);
			vScale = vec3(m_fRadius);
		}

		m_pTransform->SetScale(vScale);
	}

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void BoomEffect::Render()
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

void BoomEffect::Destroy()
{
	Engine::CGameObject::Destroy();
}

RESULT BoomEffect::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float radius)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_fRadius = radius;

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
			MSGBox("BoomEffect Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = pManager->CloneMesh(MESH_SPHERE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("BoomEffect Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_MESH);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("BoomEffect Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_SPHERE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("Player Texture Component Attach Failed!");
	}

	return PK_NOERROR;
}

BoomEffect* BoomEffect::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float radius)
{
	BoomEffect* pInstance = new BoomEffect;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, radius))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}