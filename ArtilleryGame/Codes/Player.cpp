#include "Player.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "Layer.h"
#include "Barrel.h"
#include "Texture.h"

USING(Engine)

Player::Player()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;
	m_pBarrel = nullptr;
}

Player::~Player()
{
}

void Player::Update(const _float& dt)
{
	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
	m_pBarrel->Update(dt);
}

void Player::Render()
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

	
	m_pBarrel->Render();
}

void Player::Destroy()
{
	if (nullptr != m_pBarrel)
		m_pBarrel->Destroy();

	Engine::CGameObject::Destroy();
}

void Player::LookAt(vec3 vTarget)
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

RESULT Player::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
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
			MSGBox("Player Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = pManager->CloneMesh(MESH_TANK);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("Player Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = pManager->CloneShader(SHADER_MESH);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("Player Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = pManager->CloneTexture(TEX_TANK);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("Player Texture Component Attach Failed!");
	}

	CGameObject* pGameObject = nullptr;
	//Create.Barrel
	pGameObject = Barrel::Create(sceneTag, layerTag, OBJ_PLAYER, m_pLayer,
		vec3(0.f, 1.f, 0.f), vec3(45.f, 0.f, 0.f), vec3(0.2f, 7.f, 0.2f));
	if (nullptr != pGameObject)
	{
		m_pBarrel = dynamic_cast<Barrel*>(pGameObject);
		m_pBarrel->SetParentTransform(m_pTransform);
	}

	return PK_NOERROR;
}

Player* Player::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
{
	Player* pInstance = new Player;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, pParent, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}