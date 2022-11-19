#include "ScenePirate.h"
#include "Define.h"
#include "Enums.h"

#include "XMLParser.h"
#include "Layer.h"
#include "GameObject.h"
#include "InputDevice.h"
#include "SoundMaster.h"
#include "UIManager.h"
#include "CollisionMaster.h"
#include "LightMaster.h"
#include "Light.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
#include "Transform.h"

#include "BGObject.h"
#include "Camera.h"
#include "DefaultCamera.h"
#include "TargetCamera.h"
#include <sstream>
#include <atlconv.h>

USING(Engine)
USING(glm)
USING(std)

ScenePirate::ScenePirate()
	: m_pDefaultCamera(nullptr), m_pTargetCamera(nullptr), m_pBackgroundLayer(nullptr)
	, m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pShip(nullptr), m_vStartingPosition(vec3(0.f)), m_vMiddlePosition(vec3(0.f))
	, m_vDockPosition(vec3(0.f)), m_vLeavingPosition(vec3(0.f)), m_bTargetCameraMode(false)
	, m_pSmallFactory(nullptr), m_bArriving(false), m_matLighthouse(mat4x4(1.f)), m_fLightAngle(0.f)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));
	stringstream ss;
	ss << str << "\\..\\";

	m_DataPath = ss.str();
	m_ObjListFileName = "ScenePirate_mapObjects.xml";
	m_LightListFileName = "ScenePirate_lights.xml";
}

ScenePirate::~ScenePirate()
{
}

void ScenePirate::KeyCheck(const _float& dt)
{
	static _bool isF1Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F1))
	{
		if (!isF1Down)
		{
			isF1Down = true;
			SetDefaultCameraMode();
		}
	}
	else
		isF1Down = false;

	static _bool isF2Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F2))
	{
		if (!isF2Down)
		{
			isF2Down = true;
			SetTargetCameraMode();
		}
	}
	else
		isF2Down = false;

	static _bool is1Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_1))
	{
		if (!is1Down)
		{
			is1Down = true;
			m_bArriving = false;
			SetDayTimeScene();
			SetDefaultCameraMode();
			CSoundMaster::GetInstance()->StopSound("chirp");
			CSoundMaster::GetInstance()->StopSound("creaking");
		}
	}
	else
		is1Down = false;

	static _bool is2Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_2))
	{
		if (!is2Down)
		{
			is2Down = true;
			m_bArriving = true;
			SetNightTimeScene();
			CSoundMaster::GetInstance()->PlaySound("chirp");
		}
	}
	else
		is2Down = false;

	static _bool is3Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_3))
	{
		if (!is3Down)
		{
			is3Down = true;
			m_bArriving = false;
			SetShipMovingScene();
			CSoundMaster::GetInstance()->PlaySound("chirp");
		}
	}
	else
		is3Down = false;
}

void ScenePirate::Update(const _float& dt)
{
	if (nullptr != m_pUIManager)
		m_pUIManager->Update(dt);

	KeyCheck(dt);

	if (m_bTargetCameraMode)
	{
		SetTargetCameraInfo();
	}

	SetLightHouseLight(dt);

	CScene::Update(dt);
	CLightMaster::GetInstance()->SetLightInfo();
}

void ScenePirate::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

void ScenePirate::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pUIManager);

	CScene::Destroy();
}

void ScenePirate::SetDefaultCameraSavedPosition(vec3 vPos, vec3 vRot, vec3 target)
{
	m_vCameraSavedPos.x = vPos.x;
	m_vCameraSavedPos.y = vPos.y;
	m_vCameraSavedPos.z = vPos.z;

	m_vCameraSavedRot.x = vRot.x;
	m_vCameraSavedRot.y = vRot.y;
	m_vCameraSavedRot.z = vRot.z;

	m_vCameraSavedTarget.x = target.x;
	m_vCameraSavedTarget.y = target.y;
	m_vCameraSavedTarget.z = target.z;
}

void ScenePirate::ResetDefaultCameraPos()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetCameraEye(m_vCameraSavedPos);
		m_pDefaultCamera->SetCameraRot(m_vCameraSavedRot);
		m_pDefaultCamera->SetCameraTarget(m_vCameraSavedTarget);
	}
}

void ScenePirate::SetDayTimeScene()
{
	CLightMaster::GetInstance()->SetDirectionalLightPower(2.0f);
	CLightMaster::GetInstance()->TurnOffAllLightsExceptDirectional();

	if (nullptr != m_pShip)
		m_pShip->SetEnable(false);
}

void ScenePirate::SetNightTimeScene()
{
	CLightMaster::GetInstance()->SetDirectionalLightPower(0.3f);
	CLightMaster::GetInstance()->TurnOnAllLightsExceptDirectional();
	CLightMaster::GetInstance()->TurnOffLight("SpotLightToShip");

	if (nullptr != m_pShip)
	{
		m_pShip->SetMoving(m_vStartingPosition, m_vDockPosition, m_bArriving, true, m_vMiddlePosition);
		m_pShip->SetEnable(true);
	}
}

void ScenePirate::SetShipMovingScene()
{
	CLightMaster::GetInstance()->SetDirectionalLightPower(0.3f);
	CLightMaster::GetInstance()->TurnOnAllLightsExceptDirectional();
	CLightMaster::GetInstance()->TurnOffLight("SpotLightToShip");

	if (nullptr != m_pShip)
	{
		m_pShip->SetMoving(m_vDockPosition, m_vLeavingPosition, m_bArriving);
		m_pShip->SetEnable(true);
	}
}

void ScenePirate::SetDefaultCameraMode()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetEnable(true);
		m_pDefaultCamera->SetMouseEnable(true);
	}
	if (nullptr != m_pTargetCamera)
		m_pTargetCamera->SetEnable(false);
	m_bTargetCameraMode = false;
	ResetDefaultCameraPos();
}

void ScenePirate::SetTargetCameraMode()
{
	if (nullptr == m_pShip)
		return;

	if (m_pShip->GetEnable())
	{
		if (nullptr != m_pDefaultCamera)
		{
			m_pDefaultCamera->SetEnable(false);
			m_pDefaultCamera->SetMouseEnable(false);
		}
		if (nullptr != m_pTargetCamera)
			m_pTargetCamera->SetEnable(true);
		m_bTargetCameraMode = true;
	}
	else
	{
		SetDefaultCameraMode();
	}
}

void ScenePirate::SetTargetCameraInfo()
{
	if (nullptr == m_pShip || nullptr == m_pTargetCamera || nullptr == m_pSmallFactory)
		return;

	if (!m_pShip->GetEnable())
	{
		SetDefaultCameraMode();
		return;
	}

	CTransform* pTransform = m_pShip->GetTransform();
	vec3 vPos = pTransform->GetPosition();
	vec3 vRight = pTransform->GetRightVector();
	vPos += vRight * 1.7f;
	vec3 vLook = pTransform->GetLookVector();
	vPos += -vLook * 0.8f;
	vPos.y += 1.7f;
	m_pTargetCamera->SetCameraEye(vPos);

	vec3 vTarget = m_pSmallFactory->GetPosition();
	m_pTargetCamera->SetCameraTarget(vTarget);
}

void ScenePirate::SetLightHouseLight(const _float& dt)
{
	m_fLightAngle += dt * 100.f;
	m_matLighthouse = mat4x4(1.f);
	m_matLighthouse = rotate(m_matLighthouse, radians(m_fLightAngle), vec3(0.f, 1.f, 0.f));
	vec3 vLook = normalize(m_matLighthouse[2]);
	vLook.y -= 2.f;
	vLook = normalize(vLook);
	CLightMaster::GetInstance()->SetLightDirection("SpotLight_Lighthouse", vLook);
}

RESULT ScenePirate::Ready()
{
	RESULT result = PK_NOERROR;

	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	if (nullptr != m_pUIManager)
		m_pUIManager->Ready(this);

	CSoundMaster::GetInstance()->PlaySound("wave");

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("ColorShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID); 
	if (nullptr != m_pTargetCamera)
		m_pTargetCamera->SetShaderLocation(shaderID);

	SetDayTimeScene();

	return PK_NOERROR;
}

RESULT ScenePirate::ReadyLayerAndGameObject()
{
	CLayer* pLayer = nullptr;
	CGameObject* pGameObject = nullptr;

	//Create.Cameras
	pLayer = GetLayer((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		pGameObject = DefaultCamera::Create((_uint)SCENE_PIRATE, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			m_pDefaultCamera = dynamic_cast<DefaultCamera*>(pGameObject);
		}

		pGameObject = TargetCamera::Create((_uint)SCENE_PIRATE, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			m_pTargetCamera = dynamic_cast<TargetCamera*>(pGameObject);
			m_pTargetCamera->SetEnable(false);
		}
	}

	//Create.BackgroundLayer 
	LoadBackgroundObjects();

	return PK_NOERROR;
}

ScenePirate* ScenePirate::Create()
{
	ScenePirate* pInstance = new ScenePirate();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void ScenePirate::LoadBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	CGameObject* pGameObject = nullptr;

	if (nullptr != pLayer)
	{
		m_pBackgroundLayer = pLayer;
		pLayer->RemoveAllGameObject();
		vector<CXMLParser::sObjectData> vecObjects;
		CXMLParser::sObjectData cameraData;
		CXMLParser::GetInstance()->LoadMapObjectData(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
		vector<CXMLParser::sObjectData>::iterator iter;
		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = BGObject::Create((_uint)SCENE_PIRATE, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->POSITION, iter->ROTATION, iter->SCALE, iter->SOUNDTAG);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			dynamic_cast<BGObject*>(pGameObject)->SetLock(iter->LOCK);
			if (!strcmp(iter->ID.c_str(), "PirateShip"))
			{
				m_pShip = dynamic_cast<BGObject*>(pGameObject);
				m_vDockPosition = m_pShip->GetPosition();

				CTransform* pTransform = m_pShip->GetTransform();
				vec3 vLook = pTransform->GetLookVector();
				vec3 vRight = pTransform->GetRightVector();
				m_vMiddlePosition = m_vDockPosition + (-vLook * 50.f);
				m_vStartingPosition = m_vMiddlePosition + (vRight * 50.f) + (-vLook * 50.f);
				m_vLeavingPosition = m_vDockPosition + (vLook * 200.f) + (-vRight * 100.f);
			}
			else if (!strcmp(iter->ID.c_str(), "SmallFactory"))
			{
				m_pSmallFactory = dynamic_cast<BGObject*>(pGameObject);
			}
		}
		vecObjects.clear();

		if (nullptr != m_pDefaultCamera)
		{
			SetDefaultCameraSavedPosition(cameraData.POSITION, cameraData.ROTATION, cameraData.SCALE);
			m_pDefaultCamera->SetCameraEye(cameraData.POSITION);
			m_pDefaultCamera->SetCameraRot(cameraData.ROTATION);
			m_pDefaultCamera->SetCameraTarget(cameraData.SCALE);
		}
	}
}