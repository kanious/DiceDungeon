#include "Barrel.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "BattleManager.h"
#include "ConsoleManager.h"
#include "Renderer.h"
#include "InputDevice.h"
#include "Layer.h"
#include "Texture.h"
#include "Bullet.h"
#include "BounceBullet.h"
#include "DividedBullet.h"
#include "MultipleBullet.h"
#include "BombingBullet.h"
#include "RollingBullet.h"

USING(Engine)

Barrel::Barrel()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pInputDevice = CInputDevice::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;
	m_pDiffTexture = nullptr;
	m_eBulletType = B_BOUNCE;
}

Barrel::~Barrel()
{
}

void Barrel::Update(const _float& dt)
{
	KeyCheck(dt);

	Engine::CGameObject::Update(dt);
	m_pRenderer->AddRenderObj(this);
}

void Barrel::Render()
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

void Barrel::Destroy()
{
	Engine::CGameObject::Destroy();
}

void Barrel::SetParentTransform(CTransform* parent)
{
	if (nullptr != m_pTransform)
		m_pTransform->SetParent(parent);
}

void Barrel::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pTransform)
		return;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_W))
	{
		vec3 vRot = m_pTransform->GetRotation();
		vRot.x -= dt * 25.f;
		if (vRot.x <= 5.f)
			vRot.x = 5.f;
		m_pTransform->SetRotation(vRot);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_S))
	{
		vec3 vRot = m_pTransform->GetRotation();
		vRot.x += dt * 25.f;
		if (vRot.x >= 85.f)
			vRot.x = 85.f;
		m_pTransform->SetRotation(vRot);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_A))
	{
		vec3 vRot = m_pTransform->GetRotation();
		vRot.y += dt * 25.f;
		if (vRot.y >= 360.f)
			vRot.y -= 360.f;
		m_pTransform->SetRotation(vRot);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_D))
	{
		vec3 vRot = m_pTransform->GetRotation();
		vRot.y -= dt * 25.f;
		if (vRot.y <= 0.f)
			vRot.y += 360.f;
		m_pTransform->SetRotation(vRot);
	}

	static _bool isSpaceDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_SPACE))
	{
		if (!isSpaceDown)
		{
			isSpaceDown = true;
			FireBullet();
		}
	}
	else
		isSpaceDown = false;

	static _bool is1Down, is2Down, is3Down, is4Down, is5Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_1))
	{
		if (!is1Down)
		{
			is1Down = true;
			m_eBulletType = B_BOUNCE;
			ConsoleManager::GetInstance()->ChangeBulletType(m_eBulletType);
		}
	}
	else
		is1Down = false;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_2))
	{
		if (!is2Down)
		{
			is2Down = true;
			m_eBulletType = B_DIVIDED;
			ConsoleManager::GetInstance()->ChangeBulletType(m_eBulletType);
		}
	}
	else
		is2Down = false;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_3))
	{
		if (!is3Down)
		{
			is3Down = true;
			m_eBulletType = B_MULTIPLE;
			ConsoleManager::GetInstance()->ChangeBulletType(m_eBulletType);
		}
	}
	else
		is3Down = false;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_4))
	{
		if (!is4Down)
		{
			is4Down = true;
			m_eBulletType = B_BOMBING;
			ConsoleManager::GetInstance()->ChangeBulletType(m_eBulletType);
		}
	}
	else
		is4Down = false;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_5))
	{
		if (!is5Down)
		{
			is5Down = true;
			m_eBulletType = B_ROLLING;
			ConsoleManager::GetInstance()->ChangeBulletType(m_eBulletType);
		}
	}
	else
		is5Down = false;
}

void Barrel::FireBullet()
{
	if (BattleManager::GetInstance()->GetCanFire())
		return;

	if (nullptr == m_pLayer || nullptr == m_pTransform)
		return;
	
	BattleManager::GetInstance()->SetIsFiring(true);

	vec3 vPos = m_pTransform->GetPositionWithParent();
	vec3 vRot = vec3(0.f, m_pTransform->GetRotationWithParent().y, 0.f);
	vec3 vScale = vec3(0.3f);
	CGameObject* pGameObject = nullptr;
	vec3 vForce = vec3(0.f, -0.098f, 0.f);

	switch (m_eBulletType)
	{
	case B_BOUNCE:
	{
		pGameObject = BounceBullet::Create((eSCENETAG)m_sceneTag,
			LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radians(90.f - m_pTransform->GetRotationX()));
		if (nullptr != pGameObject)
		{
			m_pLayer->AddGameObject(pGameObject);
			pGameObject->SetParentLayer(m_pLayer);
			dynamic_cast<BounceBullet*>(pGameObject)->SetForce(vForce);
		}
		break;
	}
	case B_DIVIDED:
	{
		pGameObject = DividedBullet::Create((eSCENETAG)m_sceneTag,
			LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radians(90.f - m_pTransform->GetRotationX()), true);
		if (nullptr != pGameObject)
		{
			m_pLayer->AddGameObject(pGameObject);
			pGameObject->SetParentLayer(m_pLayer);
			dynamic_cast<DividedBullet*>(pGameObject)->SetForce(vForce);
		}
		break;
	}
	case B_MULTIPLE:
	{
		pGameObject = MultipleBullet::Create((eSCENETAG)m_sceneTag,
			LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radians(90.f - m_pTransform->GetRotationX()));
		if (nullptr != pGameObject)
		{
			m_pLayer->AddGameObject(pGameObject);
			pGameObject->SetParentLayer(m_pLayer);
			dynamic_cast<MultipleBullet*>(pGameObject)->SetForce(vForce);
		}
		break;
	}
	case B_BOMBING:
		pGameObject = BombingBullet::Create((eSCENETAG)m_sceneTag,
			LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radians(90.f - m_pTransform->GetRotationX()));
		if (nullptr != pGameObject)
		{
			m_pLayer->AddGameObject(pGameObject);
			pGameObject->SetParentLayer(m_pLayer);
			dynamic_cast<BombingBullet*>(pGameObject)->SetForce(vForce);
		}
		break;

	case B_ROLLING:
		pGameObject = RollingBullet::Create((eSCENETAG)m_sceneTag,
			LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radians(90.f - m_pTransform->GetRotationX()));
		if (nullptr != pGameObject)
		{
			m_pLayer->AddGameObject(pGameObject);
			pGameObject->SetParentLayer(m_pLayer);
			dynamic_cast<RollingBullet*>(pGameObject)->SetForce(vForce);
		}
		break;
	}
}

RESULT Barrel::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_pLayer = pParent;

	RESULT result = PK_NOERROR;
	CComponent* pComponent = nullptr;

	//Clone.Transform
	pComponent = ComponentManager::GetInstance()->CloneTransform();
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
			MSGBox("Barrel Transform Component Attach Failed!");
	}

	//Clone.Mesh
	pComponent = CMesh::Create("", vec3(0.f, 1.f, 1.f));
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("Barrel Mesh Component Attach Failed!");
	}

	//Clone.Shader
	pComponent = ComponentManager::GetInstance()->CloneShader(SHADER_BASE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("Barrel Shader Component Attach Failed!");
	}

	//Clone.Texture
	pComponent = ComponentManager::GetInstance()->CloneTexture(TEX_BARREL);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_TEXTURE, pComponent);
		if (PK_NOERROR == result)
			m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
		else
			MSGBox("Barrel Texture Component Attach Failed!");
	}

	return PK_NOERROR;
}

Barrel* Barrel::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CLayer* pParent, vec3 vPos, vec3 vRot, vec3 vScale)
{
	Barrel* pInstance = new Barrel;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, pParent, vPos, vRot, vScale))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}