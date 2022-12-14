#include "DefaultCamera.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Camera.h"
#include "Transform.h"
#include "OpenGLDefines.h"
#include "InputDevice.h"
#include "SoundMaster.h"

USING(Engine)
USING(glm)

DefaultCamera::DefaultCamera()
	: m_pCamera(nullptr), m_bMouseEnable(true), m_fAngleY(0.f), m_fAngleX(0.f)
	, m_fSpeed(10.f), m_fRotSpeed(10.f)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

DefaultCamera::~DefaultCamera()
{
}

vec3 DefaultCamera::GetCameraEye()
{
	if (nullptr == m_pCamera)
		return vec3(0.f);

	return m_pCamera->GetCameraEye();
}

vec3 DefaultCamera::GetCameraRot()
{
	if (nullptr == m_pTransform)
		return vec3(0.f);

	return m_pTransform->GetRotation();
}

vec3 DefaultCamera::GetCameraTarget()
{
	if (nullptr == m_pCamera)
		return vec3(0.f);

	return m_pCamera->GetCameraTarget();
}

void DefaultCamera::SetMouseEnable(_bool enable)
{
	m_bMouseEnable = enable;

	//if (enable)
	//	m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_NORMAL);
	//else
	//	m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_HIDDEN);

	m_pInputDevice->InitMousePos();
}

void DefaultCamera::SetCameraEye(glm::vec3 eye)
{
	if (nullptr == m_pCamera || nullptr == m_pTransform || nullptr == m_pInputDevice)
		return;

	m_pTransform->SetPosition(eye);
	m_pCamera->SetCameraEye(eye);
}

void DefaultCamera::SetCameraRot(glm::vec3 rot)
{
	if (nullptr == m_pTransform)
		return;

	m_pTransform->SetRotation(rot);
}

void DefaultCamera::SetCameraTarget(glm::vec3 target)
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

void DefaultCamera::SetShaderLocation(_uint shaderID)
{
	m_shaderLocation = glGetUniformLocation(shaderID, "eyeLocation");
}

void DefaultCamera::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pTransform)
		return;

	if (!m_bMouseEnable)
	{
		vec2 vMousePos = m_pInputDevice->GetMouseMovedDistance();
		m_fAngleX += vMousePos.y;
		m_fAngleY -= vMousePos.x;

		if (m_fAngleX > 89.f)
			m_fAngleX = 89.f;
		if (m_fAngleX < -89.f)
			m_fAngleX = -89.f;

		if (m_fAngleY > 360.f)
			m_fAngleY -= 360.f;
		if (m_fAngleY < 0)
			m_fAngleY += 360.f;

		m_pTransform->SetRotationX(m_fAngleX);
		m_pTransform->SetRotationY(m_fAngleY);

		m_pTransform->Update(0);
		vec3 vDir = m_pTransform->GetLookVector();

		m_pCamera->SetCameraTarget(m_pTransform->GetPosition() + vDir);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_W))
	{
		vec3 vDir = m_pTransform->GetLookVector();
		vec3 vPos = m_pTransform->GetPosition();
		vPos += vDir * dt * m_fSpeed;
		vPos = CheckBoundary(vPos);
		
		m_pTransform->SetPosition(vPos);
		m_pCamera->SetCameraEye(vPos);
		m_pCamera->SetCameraTarget(vPos + vDir);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_S))
	{
		vec3 vDir = m_pTransform->GetLookVector();
		vec3 vPos = m_pTransform->GetPosition();
		vPos += vDir * dt * -m_fSpeed;
		vPos = CheckBoundary(vPos);

		m_pTransform->SetPosition(vPos);
		m_pCamera->SetCameraEye(vPos);
		m_pCamera->SetCameraTarget(vPos + vDir);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_A))
	{
		vec3 vRight = m_pTransform->GetRightVector();
		vec3 vPos = m_pTransform->GetPosition();
		vPos += vRight * dt * m_fSpeed;
		vPos = CheckBoundary(vPos);

		m_pTransform->SetPosition(vPos);
		m_pCamera->SetCameraEye(vPos);
		m_pCamera->SetCameraTarget(vPos + m_pTransform->GetLookVector());
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_D))
	{
		vec3 vRight = m_pTransform->GetRightVector();
		vec3 vPos = m_pTransform->GetPosition();
		vPos += vRight * dt * -m_fSpeed;
		vPos = CheckBoundary(vPos);

		m_pTransform->SetPosition(vPos);
		m_pCamera->SetCameraEye(vPos);
		m_pCamera->SetCameraTarget(vPos + m_pTransform->GetLookVector());
	}
}

vec3 DefaultCamera::CheckBoundary(vec3 vPos)
{
	if (vPos.x > 50.f)
		vPos.x = 50.f;
	else if (vPos.x < -50.f)
		vPos.x = -50.f;

	if (vPos.z > 50.f)
		vPos.z = 50.f;
	else if (vPos.z < -50.f)
		vPos.z = -50.f;

	vPos.y = 4.f;

	return vPos;
}

void DefaultCamera::Update(const _float& dt)
{
	KeyCheck(dt);
	CGameObject::Update(dt);

	vec3 vEye = m_pCamera->GetCameraEye();
	glUniform4f(m_shaderLocation, vEye.x, vEye.y, vEye.z, 1.0f);
	CSoundMaster::GetInstance()->SetListener(vEye.x, vEye.y, vEye.z);
}

void DefaultCamera::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

RESULT DefaultCamera::Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
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

	m_pInputDevice->SetCustomCrosshair();
	m_pInputDevice->SetMousePosFixed(true);
	SetMouseEnable(false);

	return PK_NOERROR;
}

DefaultCamera* DefaultCamera::Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, vec3 vPos, vec3 vRot, vec3 vScale, _float fov, _float fNear, _float fFar)
{
	DefaultCamera* pInstance = new DefaultCamera();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, vPos, vRot, vScale, fov, fNear, fFar))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}