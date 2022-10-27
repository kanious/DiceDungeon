#include "Player.h"
#include "Component.h"
#include "Transform.h"
#include "MeshManager.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "Renderer.h"
#include "InputDevice.h"

USING(Engine)

Player::Player()
{
	m_pInputDevice = CInputDevice::GetInstance();
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
}

Player::~Player()
{
}

void Player::Update(const _float& dt)
{
	KeyCheck(dt);

	Engine::CGameObject::Update(dt);

	m_pRenderer->AddRenderObj(this);
}

void Player::Render()
{
	const mat4x4 matWorld = *m_pTransform->GetWorldMatrix();
	const mat4x4 matView = m_pConfigManager->GetViewMatrix();
	const mat4x4 matProj = m_pConfigManager->GetProjMatrix();
	m_pShader->SetMatrixInfo(matWorld, matView, matProj);

	Engine::CGameObject::Render();
}

void Player::Destroy()
{
	Engine::CGameObject::Destroy();
}

void Player::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice)
		return;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_W))
	{
		m_pTransform->AddPosition(vec3(0.f, 0.f, 1.f * dt * 10.f));
	}
}

RESULT Player::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);

	RESULT result = PK_NOERROR;
	CComponent* pComponent = nullptr;

	//Create.Transform
	pComponent = CTransform::Create();
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TRANSFORM, pComponent);
		if (PK_NOERROR == result)
		{
			m_pTransform = dynamic_cast<CTransform*>(pComponent);
			m_pTransform->SetPosition(vec3(0.f, 0.f, -20.f));
			m_pTransform->SetRotation(vec3(0.f, 45.f, 0.f));
		}
		else
			MSGBox("Player Transform Component Attach Failed!");
	}

	//Create.Mesh
	pComponent = CMesh::Create("");
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("Player Mesh Component Attach Failed!");
	}

	//Create.Shader
	pComponent = CShader::Create();
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
		{
			m_pShader = dynamic_cast<CShader*>(pComponent);
			m_pShader->AddShader(GL_VERTEX_SHADER, "Shader\\BaseVertex.fx");
			m_pShader->AddShader(GL_FRAGMENT_SHADER, "Shader\\BaseFragment.fx");
			m_pShader->SetLocation();
		}
		else
			MSGBox("Player Shader Component Attach Failed!");
	}

	return PK_NOERROR;
}

Player* Player::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag)
{
	Player* pInstance = new Player;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}