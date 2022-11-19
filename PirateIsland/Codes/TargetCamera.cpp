#include "TargetCamera.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Camera.h"
#include "Transform.h"
#include "OpenGLDefines.h"
#include "InputDevice.h"
#include "UIManager.h"

USING(Engine)
USING(glm)

TargetCamera::TargetCamera()
	: m_pCamera(nullptr), m_bMouseEnable(true), m_fAngleY(0.f), m_fAngleX(0.f)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

TargetCamera::~TargetCamera()
{
}

vec3 TargetCamera::GetCameraEye()
{
	if (nullptr == m_pCamera)
		return vec3(0.f);

	return m_pCamera->GetCameraEye();
}

vec3 TargetCamera::GetCameraRot()
{
	if (nullptr == m_pTransform)
		return vec3(0.f);

	return m_pTransform->GetRotation();
}

vec3 TargetCamera::GetCameraTarget()
{
	if (nullptr == m_pCamera)
		return vec3(0.f);

	return m_pCamera->GetCameraTarget();
}

void TargetCamera::SetMouseEnable(_bool enable)
{
	m_bMouseEnable = enable;
	if (enable)
		m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_NORMAL);
	else
		m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_DISABLED);

	m_pInputDevice->InitMousePos();
}

void TargetCamera::SetCameraEye(glm::vec3 eye)
{
	if (nullptr == m_pCamera || nullptr == m_pTransform || nullptr == m_pInputDevice)
		return;

	m_pTransform->SetPosition(eye);
	m_pCamera->SetCameraEye(eye);
}

void TargetCamera::SetCameraRot(glm::vec3 rot)
{
	if (nullptr == m_pTransform)
		return;

	m_pTransform->SetRotation(rot);
}

void TargetCamera::SetCameraTarget(glm::vec3 target)
{
	if (nullptr == m_pCamera || nullptr == m_pTransform || nullptr == m_pInputDevice)
		return;

	vec3 vEye = m_pCamera->GetCameraEye();
	vec3 vDir = target - vEye;
	vDir = normalize(vDir);
	m_pCamera->SetCameraTarget(vEye + vDir);
	m_pInputDevice->InitMousePos();

	m_fAngleX = m_pTransform->GetRotationX();
	m_fAngleY = m_pTransform->GetRotationY();
}

void TargetCamera::SetShaderLocation(_uint shaderID)
{
	m_shaderLocation = glGetUniformLocation(shaderID, "eyeLocation");
}

void TargetCamera::Update(const _float& dt)
{
	if (!m_bEnable)
		return;

	CGameObject::Update(dt);

	vec3 vEye = m_pCamera->GetCameraEye();
	glUniform4f(m_shaderLocation, vEye.x, vEye.y, vEye.z, 1.0f);
}

void TargetCamera::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

RESULT TargetCamera::Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Clone.Camera
	m_pCamera = CloneComponent<CCamera*>("Camera");
	AttachComponent("Camera", m_pCamera);

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosition(vPos);
		m_pTransform->SetRotation(vRot);
		m_pTransform->SetScale(vScale);
		m_pTransform->Update(0);

		if (nullptr != m_pCamera)
			m_pCamera->InitCameraSetting(m_pTransform->GetPosition(), m_pTransform->GetLookVector(), vec3(0.f, 1.f, 0.f), fov, fNear, fFar);
	}
	return PK_NOERROR;
}

TargetCamera* TargetCamera::Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
{
	TargetCamera* pInstance = new TargetCamera();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, vPos, vRot, vScale, fov, fNear, fFar))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
