#include "Scene3DSound.h"
#include "Enums.h"
#include "Function.h"
#include "glm\vec3.hpp"
#include "InputDevice.h"
#include "DefaultCamera.h"
#include <sstream>
#include <atlconv.h>
#include "XMLParser.h"
#include "Layer.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
#include "LightMaster.h"
#include "BGObject.h"
#include "Character.h"
#include "CollisionMaster.h"
#include "Define.h"


USING(Engine)
USING(glm)
USING(std)

Scene3DSound::Scene3DSound()
	: m_pDefaultCamera(nullptr), m_pTerrain(nullptr), m_fLightCoolTime(0.5f), m_fElapseTime(0.f), m_bRed(true)
	, m_bDebugSphere(false)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));
	stringstream ss;
	ss << str << "\\..\\";

	m_DataPath = ss.str();
	m_ObjListFileName = "Scene3DSound_mapObjects.xml";
	m_LightListFileName = "Scene3DSound_lights.xml";
}

Scene3DSound::~Scene3DSound()
{
}

void Scene3DSound::Update(const _float& dt)
{
	m_fElapseTime += dt;
	if (m_fElapseTime >= m_fLightCoolTime)
	{
		m_fElapseTime = 0.f;
		if (m_bRed)
		{
			m_bRed = false;
			CLightMaster::GetInstance()->TurnOffLight("PointLight_Red");
			CLightMaster::GetInstance()->TurnOnLight("PointLight_Blue");
		}
		else
		{
			m_bRed = true;
			CLightMaster::GetInstance()->TurnOnLight("PointLight_Red");
			CLightMaster::GetInstance()->TurnOffLight("PointLight_Blue");
		}
	}
	CLightMaster::GetInstance()->SetLightInfo();

	static _bool isF1Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F1))
	{
		if (!isF1Down)
		{
			isF1Down = true;
			m_bDebugSphere = !m_bDebugSphere;

			CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
			list<CGameObject*>* pList = pLayer->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = pList->begin(); iter != pList->end(); ++iter)
			{
				dynamic_cast<BGObject*>(*iter)->SetDebugSphere(m_bDebugSphere);
			}
		}
	}
	else
		isF1Down = false;

	CScene::Update(dt);
}

void Scene3DSound::Render()
{
}

void Scene3DSound::Destroy()
{
	SafeDestroy(m_pInputDevice);
	CScene::Destroy();
}

RESULT Scene3DSound::Ready()
{
	RESULT result = PK_NOERROR;
	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	CLightMaster::GetInstance()->TurnOnLight("PointLight_Red");
	CLightMaster::GetInstance()->TurnOffLight("PointLight_Blue");

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID);

	return PK_NOERROR;
}

RESULT Scene3DSound::ReadyLayerAndGameObject()
{
	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		CGameObject* pGameObject = DefaultCamera::Create((_uint)SCENE_CAR, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			m_pDefaultCamera = dynamic_cast<DefaultCamera*>(pGameObject);
		}
	}

	//Create.BackgroundLayer 
	LoadBackgroundObjects();

	return PK_NOERROR;
}

Scene3DSound* Scene3DSound::Create()
{
	Scene3DSound* pInstance = new Scene3DSound();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void Scene3DSound::LoadBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	CGameObject* pGameObject = nullptr;

	if (nullptr != pLayer)
	{
		pLayer->RemoveAllGameObject();
		vector<CXMLParser::sObjectData> vecObjects;
		CXMLParser::sObjectData cameraData;
		CXMLParser::GetInstance()->LoadMapObjectData(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
		vector<CXMLParser::sObjectData>::iterator iter;
 		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = BGObject::Create((_uint)SCENE_CAR, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->POSITION, iter->ROTATION, iter->SCALE, iter->SOUNDTAG);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
		}
		vecObjects.clear();

		if (nullptr != m_pDefaultCamera)
		{
			m_pDefaultCamera->SetCameraEye(cameraData.POSITION);
			m_pDefaultCamera->SetCameraRot(cameraData.ROTATION);
			m_pDefaultCamera->SetCameraTarget(cameraData.SCALE);
		}
	}
}