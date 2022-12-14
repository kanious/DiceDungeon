#include "SceneFPS.h"
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
#include "Target.h"
#include "Define.h"
#include "PhysicsSystem.h"
#include "ParticleSystem.h"
#include "UIManager.h"
#include "Enums.h"
#include "SkyBox.h"
#include "Renderer.h"


USING(Engine)
USING(glm)
USING(std)

SceneFPS::SceneFPS()
	: m_pDefaultCamera(nullptr), m_pObjLayer(nullptr), m_pSkyBox(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);
	m_pPhysicsSystem = PhysicsSystem::GetInstance(); m_pPhysicsSystem->AddRefCnt();
	m_pParticleSystem = ParticleSystem::GetInstance(); m_pParticleSystem->AddRefCnt();
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();
	

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));//
	stringstream ss;
	ss << str << "\\..\\";

	m_DataPath = ss.str();
	m_ObjListFileName = "FPSTraining_mapObjects.xml";
	m_LightListFileName = "FPSTraining_lights.xml";
}

SceneFPS::~SceneFPS()
{
}

void SceneFPS::KeyCheck()
{
	static _bool isF1Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F1))
	{
		if (!isF1Down)
		{
			isF1Down = true;

			if (SHOT_MODE == m_pPhysicsSystem->GetGameMode())
				m_pPhysicsSystem->SetGameMode(HOVER_MODE);
			else
				m_pPhysicsSystem->SetGameMode(SHOT_MODE);
		}
	}
	else
		isF1Down = false;
}

void SceneFPS::Update(const _float& dt)
{
	if (nullptr != m_pSkyBox)
		CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	CLightMaster::GetInstance()->SetLightInfo();

	KeyCheck();

	if (nullptr != m_pPhysicsSystem)
		m_pPhysicsSystem->Update(dt);

	if (nullptr != m_pParticleSystem)
		m_pParticleSystem->Update(dt);

	CScene::Update(dt);
}

void SceneFPS::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

void SceneFPS::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pPhysicsSystem);
	SafeDestroy(m_pParticleSystem);
	SafeDestroy(m_pUIManager);
	SafeDestroy(m_pSkyBox);

	CScene::Destroy();
}

RESULT SceneFPS::Ready()
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

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID);

	if (nullptr != m_pPhysicsSystem)
		m_pPhysicsSystem->Ready(this, m_pDefaultCamera);

	if (nullptr != m_pParticleSystem)
		m_pParticleSystem->Ready(this);

	if (nullptr != m_pUIManager)
		m_pUIManager->Ready();

	if (nullptr == m_pSkyBox)
	{
		stringstream ss, ss2;
		ss << m_DataPath << "Assets\\textures\\SkyBox\\";

		vector<string> faces;
		ss2.str(""); ss2 << ss.str() << "right.jpg"; faces.push_back(ss2.str());
		ss2.str(""); ss2 << ss.str() << "left.jpg"; faces.push_back(ss2.str());
		ss2.str(""); ss2 << ss.str() << "top.jpg"; faces.push_back(ss2.str());
		ss2.str(""); ss2 << ss.str() << "bottom.jpg"; faces.push_back(ss2.str());
		ss2.str(""); ss2 << ss.str() << "front.jpg"; faces.push_back(ss2.str());
		ss2.str(""); ss2 << ss.str() << "back.jpg"; faces.push_back(ss2.str());

		CComponent* skyboxShader = CComponentMaster::GetInstance()->FindComponent("SkyBoxShader");
		m_pSkyBox = CSkyBox::Create(faces, dynamic_cast<CShader*>(skyboxShader));
	}

	return PK_NOERROR;
}

RESULT SceneFPS::ReadyLayerAndGameObject()
{
	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		CGameObject* pGameObject = DefaultCamera::Create((_uint)SCENE_FPS, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
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

SceneFPS* SceneFPS::Create()
{
	SceneFPS* pInstance = new SceneFPS();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void SceneFPS::LoadBackgroundObjects()
{
	m_pObjLayer = GetLayer((_uint)LAYER_TARGET);
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
			if (iter->LOCK)
			{
				pGameObject = BGObject::Create((_uint)SCENE_FPS, pLayer->GetTag()
					, (_uint)OBJ_BACKGROUND, pLayer, iter->ID
					, iter->POSITION, iter->ROTATION, iter->SCALE, iter->SOUNDTAG);
				if (nullptr == pGameObject)
					continue;
				AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			}
			else
			{
				pGameObject = BGObject::Create((_uint)SCENE_FPS, m_pObjLayer->GetTag()
					, (_uint)OBJ_DYNAMIC, m_pObjLayer, iter->ID
					, iter->POSITION, iter->ROTATION, iter->SCALE, iter->SOUNDTAG);
				if (nullptr == pGameObject)
					continue;
				AddGameObjectToLayer(m_pObjLayer->GetTag(), pGameObject);
				m_pPhysicsSystem->AddBGObject(dynamic_cast<BGObject*>(pGameObject));
			}
		}
		vecObjects.clear();

		if (nullptr != m_pDefaultCamera)
		{
			m_pDefaultCamera->SetCameraEye(cameraData.POSITION);
			m_pDefaultCamera->SetCameraRot(cameraData.ROTATION);
			m_pDefaultCamera->SetCameraTarget(cameraData.SCALE);
		}

		// Add Random Targets
		_float xRand = 0.f;
		_float zRand = 0.f;
		for (int i = 0; i < 15; ++i)
		{
			xRand = GetRandNum(-4900, 4900) / 100.f;
			zRand = GetRandNum(-4900, 4900) / 100.f;
			
			pGameObject = Target::Create((_uint)SCENE_FPS, m_pObjLayer->GetTag()
				, (_uint)OBJ_DYNAMIC, m_pObjLayer
				, "Sphere", vec3(xRand, SPHERE_HEIGHT, zRand), vec3(0.f), vec3(0.1f), "");
			if (nullptr == pGameObject)
					continue;
			AddGameObjectToLayer(m_pObjLayer->GetTag(), pGameObject);
			m_pPhysicsSystem->AddTarget(dynamic_cast<Target*>(pGameObject));
		}
	}
}