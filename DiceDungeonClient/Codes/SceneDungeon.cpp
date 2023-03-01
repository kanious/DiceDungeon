#include "SceneDungeon.h"
#include "Function.h"
#include "glm\vec3.hpp"
#include "InputDevice.h"
#include "DefaultCamera.h"
#include "JsonParser.h"
#include "Layer.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
#include "LightMaster.h"
#include "BGObject.h"
#include "Define.h"
#include "UIManager.h"
#include "Enums.h"
#include "SkyBox.h"
#include "Renderer.h"
#include "Player.h"
#include "AnimationManager.h"
#include "AnimationData.h"
#include "ObjectFactory.h"
#include "TargetManager.h"

#include <sstream>
#include <atlconv.h>
 

USING(Engine)
USING(glm)
USING(std)

SceneDungeon::SceneDungeon()
	: m_pSkyBox(nullptr)
	, m_pDefaultCamera(nullptr), m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pCharacterLayer(nullptr)	, m_bFollowingMode(false)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();
	m_pAnimationManager = AnimationManager::GetInstance(); m_pAnimationManager->AddRefCnt();
	m_pTargetManager = TargetManager::GetInstance(); m_pTargetManager->AddRefCnt();

	m_ObjListFileName = "mapObjects.json";
	m_LightListFileName = "lights.xml";
}

SceneDungeon::~SceneDungeon()
{
}

// Call instead of destructor to manage class internal data
void SceneDungeon::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pUIManager);

	SafeDestroy(m_pSkyBox);
	SafeDestroy(m_pAnimationManager);
	SafeDestroy(m_pTargetManager);

	CScene::Destroy();
}

// Basic Update Function
void SceneDungeon::Update(const _float& dt)
{
	//if (nullptr != m_pSkyBox)
	//	CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	KeyCheck();

	CLightMaster::GetInstance()->SetLightInfo();

	if (nullptr != m_pAnimationManager)
		m_pAnimationManager->Update(dt);

	if (nullptr != m_pTargetManager)
		m_pTargetManager->Update(dt);

	CScene::Update(dt);
}

// Basic Render Function
void SceneDungeon::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

// Return current camera position
vec3 SceneDungeon::GetCameraPos()
{
	if (nullptr != m_pDefaultCamera)
		return m_pDefaultCamera->GetCameraEye();

	return vec3(0.f);
}

// Check User input
void SceneDungeon::KeyCheck()
{
	static _bool isF5Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F5))
	{
		if (!isF5Down)
		{
			isF5Down = true;
			ResetDefaultCameraPos();
		}
	}
	else
		isF5Down = false;
}

// Saves camera position
void SceneDungeon::SetDefaultCameraSavedPosition(vec3 vPos, vec3 vRot, vec3 target)
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

// Reset camera position
void SceneDungeon::ResetDefaultCameraPos()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetCameraEye(m_vCameraSavedPos);
		m_pDefaultCamera->SetCameraRot(m_vCameraSavedRot);
		m_pDefaultCamera->SetCameraTarget(m_vCameraSavedTarget);
	}
}

// Initialize
RESULT SceneDungeon::Ready(string dataPath)
{
	m_DataPath = dataPath;

	// AnimationData
	CAnimationData::GetInstance()->LoadAnimations(m_DataPath);

	// GameObjects
	RESULT result = PK_NOERROR;
	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("MeshShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	// Set Camera info to Shader
	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->AddShaderLocation("MeshShader");

	// UI
	if (nullptr != m_pUIManager)
		m_pUIManager->Ready();

	// TargetManager
	if (nullptr != m_pTargetManager)
		m_pTargetManager->Ready(this);

	//if (nullptr == m_pSkyBox)
	//{
	//	stringstream ss, ss2;
	//	ss << m_DataPath << "Texture\\SkyBox\\";

	//	vector<string> faces;
	//	ss2.str(""); ss2 << ss.str() << "right.jpg"; faces.push_back(ss2.str());
	//	ss2.str(""); ss2 << ss.str() << "left.jpg"; faces.push_back(ss2.str());
	//	ss2.str(""); ss2 << ss.str() << "top.jpg"; faces.push_back(ss2.str());
	//	ss2.str(""); ss2 << ss.str() << "bottom.jpg"; faces.push_back(ss2.str());
	//	ss2.str(""); ss2 << ss.str() << "front.jpg"; faces.push_back(ss2.str());
	//	ss2.str(""); ss2 << ss.str() << "back.jpg"; faces.push_back(ss2.str());

	//	CComponent* skyboxShader = CComponentMaster::GetInstance()->FindComponent("SkyBoxShader");
	//	m_pSkyBox = CSkyBox::Create(faces, dynamic_cast<CShader*>(skyboxShader));
	//}

	return PK_NOERROR;
}

// Initialize GameObjects
RESULT SceneDungeon::ReadyLayerAndGameObject()
{
	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_INTERACTIVE_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);

		m_pDefaultCamera = ObjectFactory::CreateCamera(
			(_uint)SCENE_3D, pLayer->GetTag(),
			(_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
	}

	//Create.BackgroundLayer 
	LoadObjects();

	//Create.Player/AIs
	//AddCharacters();

	return PK_NOERROR;
}

// Load Objects from json file
void SceneDungeon::LoadObjects()
{
	CLayer* bgLayer = GetLayer((_uint)LAYER_BACKGROUND_OBJECT);
	//CGameObject* pGameObject = nullptr;
	m_pCharacterLayer = GetLayer((_uint)LAYER_CHARACTER);

	if (nullptr == bgLayer)
		return;

	bgLayer->RemoveAllGameObject();
	vector<CJsonParser::sObjectData> vecObjects;
	CJsonParser::sObjectData cameraData;
	CJsonParser::GetInstance()->LoadObjectList(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
	vector<CJsonParser::sObjectData>::iterator iter;
	for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
	{
		if (!strcmp("static_obj", iter->LAYERTYPE.c_str()) || !strcmp("interative_obj", iter->LAYERTYPE.c_str()))
		{
			ObjectFactory::CreateBGObject(
				(_uint)SCENE_3D,
				bgLayer->GetTag(),
				(_uint)OBJ_BACKGROUND,
				bgLayer,
				iter->ID, iter->POSITION, iter->ROTATION, iter->SCALE);
		}
		else if (!strcmp("character", iter->LAYERTYPE.c_str()))
		{
			ObjectFactory::CreatePlayer(
				(_uint)SCENE_3D,
				m_pCharacterLayer->GetTag(),
				(_uint)OBJ_CHARACTER,
				m_pCharacterLayer,
				iter->ID, iter->POSITION, iter->ROTATION, iter->SCALE);
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

// Create an instance
SceneDungeon* SceneDungeon::Create(string dataPath)
{
	SceneDungeon* pInstance = new SceneDungeon();
	if (PK_NOERROR != pInstance->Ready(dataPath))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
