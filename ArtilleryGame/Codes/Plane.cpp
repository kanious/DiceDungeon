#include "Plane.h"
#include "Component.h"
#include "Transform.h"
#include "MeshSquare.h"
#include "Shader.h"
#include "Renderer.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "Texture.h"


USING(Engine)

Plane::Plane()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMeshSqaure = nullptr;
	m_pShader = nullptr;
}

Plane::~Plane()
{
}

void Plane::Update(const _float& dt)
{
	Engine::CGameObject::Update(dt);

	m_pRenderer->AddRenderObj(this);
}

void Plane::Render()
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

void Plane::Destroy()
{
	Engine::CGameObject::Destroy();
}

RESULT Plane::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);

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
			MSGBox("Plane Transform Component Attach Failed!");
	}

	//Create.Mesh
	pComponent = CMeshSquare::Create("");
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMeshSqaure = dynamic_cast<CMeshSquare*>(pComponent);
		else
			MSGBox("Plane Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_MESH);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("Plane Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_GRID);
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

Plane* Plane::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale)
{
	Plane* pInstance = new Plane;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}