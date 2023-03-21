#include "DefaultCamera.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Camera.h"
#include "Transform.h"
#include "OpenGLDefines.h"
#include "InputDevice.h"
#include "Shader.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Player.h"

USING(Engine)
USING(glm)
USING(std)

DefaultCamera::DefaultCamera()
	: m_pCamera(nullptr), m_bMouseEnable(true), m_fAngleY(0.f), m_fAngleX(0.f)
	, m_fSpeed(40.f), m_fRotSpeed(20.f), m_pTarget(nullptr), m_fDistance(50.f)
	, m_eCurDir(CD_DOWN), m_ePrevDir(CD_DOWN), m_fHeight(20.f), m_fDist(15.f)
	, m_iZoomLevel(5), m_iZoomMinLevel(1), m_iZoomMaxLevel(10)
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
	if (enable)
		m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_NORMAL);
	else
		m_pInputDevice->SetMouseCursorMode(GLFW_CURSOR_DISABLED);

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

// Save shaders to deliver camera information
void DefaultCamera::AddShaderLocation(string shaderTag)
{
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent(shaderTag);
	if (nullptr != shader)
	{
		_uint shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
		_uint shaderLocation = glGetUniformLocation(shaderID, "eyeLocation");
		m_vecShaders.push_back(shaderLocation);
	}
}

// Check user input
void DefaultCamera::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pTransform)
		return;

	if (m_ePrevDir != m_eCurDir)
	{
		vec3 vPos = m_pCamera->GetCameraEye();
		_float fDist = distance(vPos, m_vDestPos);
		if (0.5f * (_float)m_iZoomLevel > fDist)
		{
			m_pCamera->SetCameraEye(m_vDestPos);
			m_ePrevDir = m_eCurDir;
		}
		else
		{
			vec3 vDir = m_vDestPos - vPos;
			vDir = normalize(vDir);
			vPos += vDir * dt * m_fSpeed * (_float)m_iZoomLevel;
			m_pCamera->SetCameraEye(vPos);
		}
		return;
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_W))
	{
		MoveCameraDir(dt, CD_UP);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_S))
	{
		MoveCameraDir(dt, CD_DOWN);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_A))
	{
		MoveCameraDir(dt, CD_LEFT);
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_D))
	{
		MoveCameraDir(dt, CD_RIGHT);
	}

	static _bool isQDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_Q))
	{
		if (!isQDown)
		{
			isQDown = true;
			RotateCameraDir(true);
		}
	}
	else
		isQDown = false;

	static _bool isEDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_E))
	{
		if (!isEDown)
		{
			isEDown = true;
			RotateCameraDir(false);
		}
	}
	else
		isEDown = false;

	vec2 mouseScroll = m_pInputDevice->GetMouseScrollDistance();
	if (0 != mouseScroll.y)
	{
		if (0 < mouseScroll.y)
		{
			--m_iZoomLevel;
			if (m_iZoomLevel < m_iZoomMinLevel)
				m_iZoomLevel = m_iZoomMinLevel;
		}
		else
		{
			++m_iZoomLevel;
			if (m_iZoomLevel > m_iZoomMaxLevel)
				m_iZoomLevel = m_iZoomMaxLevel;
		}

		vec3 vTarget = m_pCamera->GetCameraTarget();
		vTarget.y += m_iZoomLevel * m_fHeight;
		switch (m_eCurDir)
		{
		case CD_UP:
			vTarget.z += m_iZoomLevel * m_fDist;
			break;
		case CD_DOWN:
			vTarget.z -= m_iZoomLevel * m_fDist;
			break;
		case CD_LEFT:
			vTarget.x += m_iZoomLevel * m_fDist;
			break;
		case CD_RIGHT:
			vTarget.x -= m_iZoomLevel * m_fDist;
			break;
		}
		m_pCamera->SetCameraEye(vTarget);
	}
}

// Following Target
void DefaultCamera::FollowPlayer()
{
	if (nullptr == m_pTarget)
		return;

	vec3 vPos = m_pTarget->GetPosition();
	m_pCamera->SetCameraTarget(vPos);

	vec3 vLook = m_pTarget->GetLookVector();
	vPos -= vLook * m_fDistance;
	vPos.y += m_fDistance;

	m_pCamera->SetCameraEye(vPos);
}

void DefaultCamera::MoveCameraDir(const _float& dt, eCameraDir eDir)
{
	vec3 vEye = m_pCamera->GetCameraEye();
	vec3 vTarget = m_pCamera->GetCameraTarget();

	switch (m_eCurDir)
	{
	case CD_UP:
	{
		switch (eDir)
		{
		case CD_UP:
			vEye.z -= dt * m_fSpeed;
			vTarget.z -= dt * m_fSpeed;
			break;
		case CD_DOWN:
			vEye.z += dt * m_fSpeed;
			vTarget.z += dt * m_fSpeed;
			break;
		case CD_LEFT:
			vEye.x -= dt * m_fSpeed;
			vTarget.x -= dt * m_fSpeed;
			break;
		case CD_RIGHT:
			vEye.x += dt * m_fSpeed;
			vTarget.x += dt * m_fSpeed;
			break;
		}
	}
		break;
	case CD_DOWN:
	{
		switch (eDir)
		{
		case CD_UP:
			vEye.z += dt * m_fSpeed;
			vTarget.z += dt * m_fSpeed;
			break;
		case CD_DOWN:
			vEye.z -= dt * m_fSpeed;
			vTarget.z -= dt * m_fSpeed;
			break;
		case CD_LEFT:
			vEye.x += dt * m_fSpeed;
			vTarget.x += dt * m_fSpeed;
			break;
		case CD_RIGHT:
			vEye.x -= dt * m_fSpeed;
			vTarget.x -= dt * m_fSpeed;
			break;
		}
	}
		break;
	case CD_LEFT:
	{
		switch (eDir)
		{
		case CD_UP:
			vEye.x -= dt * m_fSpeed;
			vTarget.x -= dt * m_fSpeed;
			break;
		case CD_DOWN:
			vEye.x += dt * m_fSpeed;
			vTarget.x += dt * m_fSpeed;
			break;
		case CD_LEFT:
			vEye.z += dt * m_fSpeed;
			vTarget.z += dt * m_fSpeed;
			break;
		case CD_RIGHT:
			vEye.z -= dt * m_fSpeed;
			vTarget.z -= dt * m_fSpeed;
			break;
		}
	}
		break;
	case CD_RIGHT:
	{
		switch (eDir)
		{
		case CD_UP:
			vEye.x += dt * m_fSpeed;
			vTarget.x += dt * m_fSpeed;
			break;
		case CD_DOWN:
			vEye.x -= dt * m_fSpeed;
			vTarget.x -= dt * m_fSpeed;
			break;
		case CD_LEFT:
			vEye.z -= dt * m_fSpeed;
			vTarget.z -= dt * m_fSpeed;
			break;
		case CD_RIGHT:
			vEye.z += dt * m_fSpeed;
			vTarget.z += dt * m_fSpeed;
			break;
		}
	}
		break;
	}

	m_pCamera->SetCameraEye(vEye);
	m_pCamera->SetCameraTarget(vTarget);
}

void DefaultCamera::RotateCameraDir(_bool clockwise)
{
	m_ePrevDir = m_eCurDir;

	if (CD_UP == m_eCurDir)
	{
		if (clockwise)
		{
			m_eCurDir = CD_RIGHT;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.x -= m_iZoomLevel * m_fDist;
		}
		else
		{
			m_eCurDir = CD_LEFT;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.x += m_iZoomLevel * m_fDist;
		}
	}
	else if (CD_RIGHT == m_eCurDir)
	{
		if (clockwise)
		{
			m_eCurDir = CD_DOWN;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.z -= m_iZoomLevel * m_fDist;;
		}
		else
		{
			m_eCurDir = CD_UP;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.z += m_iZoomLevel * m_fDist;;
		}
	}
	else if (CD_DOWN == m_eCurDir)
	{
		if (clockwise)
		{
			m_eCurDir = CD_LEFT;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.x += m_iZoomLevel * m_fDist;;
		}
		else
		{
			m_eCurDir = CD_RIGHT;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.x -= m_iZoomLevel * m_fDist;;
		}
	}
	else if (CD_LEFT == m_eCurDir)
	{
		if (clockwise)
		{
			m_eCurDir = CD_UP;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.z += m_iZoomLevel * m_fDist;;
		}
		else
		{
			m_eCurDir = CD_DOWN;
			m_vDestPos = m_pCamera->GetCameraTarget();
			m_vDestPos.y += m_iZoomLevel * m_fHeight;;
			m_vDestPos.z -= m_iZoomLevel * m_fDist;;
		}
	}
}

// Basic Update Function
void DefaultCamera::Update(const _float& dt)
{
	KeyCheck(dt);
	//FollowPlayer();

	CGameObject::Update(dt);

	vec3 vEye = m_pCamera->GetCameraEye();

	for (int i = 0; i < m_vecShaders.size(); ++i)
		glUniform4f(m_vecShaders[i], vEye.x, vEye.y, vEye.z, 1.0f);
}

// Call instead of destructor to manage class internal data
void DefaultCamera::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

// Initialize
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
			m_pCamera->InitCameraSetting(m_pTransform->GetPosition(), vec3(0.f), vec3(0.f, 1.f, 0.f), fov, fNear, fFar);
	}
	return PK_NOERROR;
}

// Create an instance
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