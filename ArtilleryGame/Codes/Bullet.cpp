#include "Bullet.h"
#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Shader.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "Renderer.h"
#include "Layer.h"

USING(Engine)

Bullet::Bullet()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pTransform = nullptr;
	m_pMesh = nullptr;
	m_pShader = nullptr;

	m_fMass = 1.f;
	m_fVelocity = 2.f;
	m_fTimeElapsed = 0.f;
	m_fForce = 9.8f;
	m_fCoef = 0.8f;
	m_iBounceCount = 0;
}

Bullet::~Bullet()
{
}

void Bullet::Update(const _float& dt)
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
			m_fVelocity *= m_fCoef;
			m_iBounceCount++;
		}
		m_fTimeElapsed += dt;


		m_fTimeElapsed += dt;

		vec3 vLook = m_pTransform->GetLookVector();
		vLook = normalize(vLook);
		_float xPower = cos(m_fAngle) * m_fVelocity * dt;
		_float yPower = sin(m_fAngle) * m_fVelocity * dt - (m_fForce * m_fMass * m_fTimeElapsed * m_fTimeElapsed / 2.f);
		m_pTransform->AddPosition(vec3(vLook.x * xPower, yPower, vLook.z * xPower));



		//vec3 vPos = m_pTransform->GetPosition();
		//if (vPos.y <= 0.f)
		//{
		//	m_fTimeElapsed = 0;
		//	m_fVelocity *= m_fCoef;
		//	//m_fForce *= -1 * m_fCoef;
		//}

		//vec3 vLook = m_pTransform->GetLookVector();
		//vLook = normalize(vLook);

		//vec3 axisXY = vec3(vLook.x, vLook.y, vLook.z);
		//_float dotResult = dot(axisXY, vLook);
		//_float angle = acosf(dotResult);
		////cout << angle << endl;



		//angle = m_pTransform->GetRotation().x;
		//angle = radians(angle);
		//_float xPower = cos(angle) * m_fVelocity * m_fTimeElapsed;
		//_float yPower = sin(angle) * m_fVelocity - (m_fForce * m_fTimeElapsed * m_fTimeElapsed / 2.f);
		//m_pTransform->AddPosition(vec3(vLook.x * xPower, yPower, vLook.z * xPower));



		//_float fPower = 0 - (m_fSpeed * -1.f + (0.98f * m_fTimeElapsed * m_fTimeElapsed) / 2.f);
		//m_pTransform->AddPosition(vLook * (fPower * dt));
		//m_pTransform->AddPosition(vec3(0.f, fPower, 0.f));

		//if (xPower <= 0.1f)
		//{
		//	Boom();
		//	return;
		//}

		// 중력가속도 : 9.8
		//-sin(90) : -1
		//-> 힘 * -sin(90) + (중력가속도 * 시간 * 시간) / 2
		//_float fPower = 0 - (m_fSpeed * -1.f + (0.98f * m_fTimeElapsed * m_fTimeElapsed) / 2.f);
		//m_pTransform->AddPosition(vLook * (10 * dt));
		//m_pTransform->AddPosition(vec3(0.f, fPower, 0.f));


		// 초기 각도...
	}

	Engine::CGameObject::Update(dt);

	//vec3 vPos = m_pTransform->GetPosition();
	//cout << "Bullet: " << vPos.x << "," << vPos.y << "," << vPos.z << endl;

	m_pRenderer->AddRenderObj(this);
}

void Bullet::Render()
{
	mat4x4 matWorld = *m_pTransform->GetWorldMatrix();
	//matWorld = rotate(matWorld, radians(180.f), vec3(0.f, 1.f, 0.f));
	const mat4x4 matView = m_pConfigManager->GetViewMatrix();
	const mat4x4 matProj = m_pConfigManager->GetProjMatrix();
	m_pShader->SetMatrixInfo(matWorld, matView, matProj);

	Engine::CGameObject::Render();
}

void Bullet::Destroy()
{
	Engine::CGameObject::Destroy();
}
void Bullet::Boom()
{
	SetDead(true);
}

RESULT Bullet::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_fAngle = angle;

	RESULT result = PK_NOERROR;
	CComponent* pComponent = nullptr;

	//Create.Transform
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
			MSGBox("Bullet Transform Component Attach Failed!");
	}

	//Create.Mesh
	pComponent = CMesh::Create("", vec3(0.f, 1.f, 0.f));
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_MESH, pComponent);
		if (PK_NOERROR == result)
			m_pMesh = dynamic_cast<CMesh*>(pComponent);
		else
			MSGBox("Bullet Mesh Component Attach Failed!");
	}

	//Create.Shader
	pComponent = ComponentManager::GetInstance()->CloneShader(SHADER_BASE);
	if (nullptr != pComponent)
	{
		result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pShader = dynamic_cast<CShader*>(pComponent);
		else
			MSGBox("Bullet Shader Component Attach Failed!");
	}

	return PK_NOERROR;
}

Bullet* Bullet::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, vec3 vPos, vec3 vRot, vec3 vScale, _float angle)
{
	Bullet* pInstance = new Bullet;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, vPos, vRot, vScale, angle))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}