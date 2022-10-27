#include "TargetCamera.h"
#include "Component.h"
#include "Camera.h"
#include "ConfigurationManager.h"
#include "Transform.h"

USING(Engine)

TargetCamera::TargetCamera()
{
	m_pConfigManager = ConfigurationManager::GetInstance();
	m_pCamera = nullptr;
	m_pTarget = nullptr;
}

TargetCamera::~TargetCamera()
{
}

void TargetCamera::Update(const _float& dt)
{
	// 타겟의 위치 기반으로 카메라의 vEye, vTarget 갱신해주기

	Engine::CGameObject::Update(dt);

	if (nullptr != m_pCamera)
	{
		m_pCamera->SetRatio(m_pConfigManager->GetRatio());
		m_pConfigManager->SetViewMatrix(m_pCamera->GetViewMatrix());
		m_pConfigManager->SetProjMatrix(m_pCamera->GetProjMatrix());
	}
}

void TargetCamera::Destroy()
{
	Engine::CGameObject::Destroy();
}

void TargetCamera::SetTarget(CTransform* pTarget)
{
	return;

	if (nullptr == pTarget)
		return;

	m_pTarget = pTarget;
	
	if (nullptr != m_pCamera)
	{
		m_pCamera->SetCameraTarget(m_pTarget->GetPosition());
		// TO_DO
		//m_pCamera->SetCameraEye(m_pTarget->GetPosition());
	}
}

void TargetCamera::KeyCheck(const _float&)
{

}

RESULT TargetCamera::Ready(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CTransform* target)
{
	SetupGameObject((_uint)objTag, (_uint)layerTag, (_uint)sceneTag);
	m_pTarget = target;

	vec3 vEye, vTarget;
	if (nullptr != m_pTarget)
	{
		// TO_DO
		vTarget = m_pTarget->GetPosition();
	}
	else
	{
		// TO_DO : Test
		vEye = vec3(0.0, 25.0, -100.0f); //vec3(0.f);
		vTarget = vec3(0.f);
	}
	vec3 vUp = vec3(0.f, 1.f, 0.f);
	CComponent* pComponent = CCamera::Create(vEye, vTarget, vUp
			, ConfigurationManager::GetInstance()->GetFieldOfView()
			, ConfigurationManager::GetInstance()->GetRatio()
			, ConfigurationManager::GetInstance()->GetNear()
			, ConfigurationManager::GetInstance()->GetFar());
	if (nullptr != pComponent)
	{
		RESULT result = AttachComponent((_uint)COMP_SHADER, pComponent);
		if (PK_NOERROR == result)
			m_pCamera = dynamic_cast<CCamera*>(pComponent);
		else
			MSGBox("TargetCamera Camera Component Attach Failed!");
	}

	return PK_NOERROR;
}

TargetCamera* TargetCamera::Create(eSCENETAG sceneTag, eLAYERTAG layerTag, eOBJTAG objTag, CTransform* target)
{
	TargetCamera* pInstance = new TargetCamera;

	if (PK_NOERROR != pInstance->Ready(sceneTag, layerTag, objTag, target))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
