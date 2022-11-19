#include "SceneArena.h"
#include "Enums.h"
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

USING(Engine)
USING(glm)
USING(std)

SceneArena::SceneArena()
	: m_pDefaultCamera(nullptr)
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
	m_ShaderDataFileName = "SceneArena_shaderdataList.xml";
	m_TextureDataFileName = "SceneArena_texturedataList.xml";
	m_MeshDataFileName = "SceneArena_meshdataList.xml";
	m_ObjListFileName = "SceneArena_mapObjects.xml";
	m_LightListFileName = "SceneArena_lights.xml";
}

SceneArena::~SceneArena()
{
}

void SceneArena::Update(const _float& dt)
{
	CLightMaster::GetInstance()->SetLightInfo();

	CScene::Update(dt);
}

void SceneArena::Render()
{
}

void SceneArena::Destroy()
{
	SafeDestroy(m_pInputDevice);
	CScene::Destroy();
}

RESULT SceneArena::Ready()
{
	RESULT result = PK_NOERROR;
	result = ReadyComponent();
	if (PK_NOERROR != result)
		return result;

	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	//CXMLParser::GetInstance()->LoadSoundData(m_DataPath, m_SoundDataFileName);
	//CSoundMaster::GetInstance()->PlaySound("peaceful_night");
	//CSoundMaster::GetInstance()->PlaySound("chirp");
	//CSoundMaster::GetInstance()->PlaySound("frames");

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID);

	return PK_NOERROR;
}

RESULT SceneArena::ReadyComponent()
{
	//Create.Shader
	CXMLParser::GetInstance()->LoadShaderData(m_DataPath, m_ShaderDataFileName);

	//Create.Texture
	CXMLParser::GetInstance()->LoadTextureData(m_DataPath, m_TextureDataFileName);

	//Create.Mesh
	CXMLParser::GetInstance()->LoadMeshData(m_DataPath, m_MeshDataFileName);

	return PK_NOERROR;
}

RESULT SceneArena::ReadyLayerAndGameObject()
{
	CLayer* pLayer = nullptr;
	CGameObject* pGameObject = nullptr;

	//Create.Camera
	pLayer = GetLayer((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		pGameObject = DefaultCamera::Create((_uint)SCENE_FOREST, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
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

SceneArena* SceneArena::Create()
{
	SceneArena* pInstance = new SceneArena();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void SceneArena::LoadBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	CGameObject* pGameObject = nullptr;

	if (nullptr != pLayer)
	{
		//m_pBackgroundLayer = pLayer;
		pLayer->RemoveAllGameObject();
		vector<CXMLParser::sObjectData> vecObjects;
		CXMLParser::sObjectData cameraData;
		CXMLParser::GetInstance()->LoadMapObjectData(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
		vector<CXMLParser::sObjectData>::iterator iter;
		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = BGObject::Create((_uint)SCENE_FOREST, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->POSITION, iter->ROTATION, iter->SCALE, iter->SOUNDTAG);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			dynamic_cast<BGObject*>(pGameObject)->SetLock(iter->LOCK);
		}
		vecObjects.clear();

		if (nullptr != m_pDefaultCamera)
		{
			//SetDefaultCameraSavedPosition(cameraData.POSITION, cameraData.ROTATION, cameraData.SCALE);
			m_pDefaultCamera->SetCameraEye(cameraData.POSITION);
			m_pDefaultCamera->SetCameraRot(cameraData.ROTATION);
			m_pDefaultCamera->SetCameraTarget(cameraData.SCALE);
		}
	}
}