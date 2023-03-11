#include "Scene3D.h"
#include "Function.h"
#include "glm\vec3.hpp"
#include "InputDevice.h"
#include "DefaultCamera.h"
#include "JsonParser.h"
#include "Layer.h"
#include "CollisionMaster.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
#include "LightMaster.h"
#include "BGObject.h"
#include "Define.h"
#include "UIManager.h"
#include "SkyBox.h"
#include "Renderer.h"
#include "Player.h"
#include "ObjectFactory.h"

#include <sstream>
#include <atlconv.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


USING(Engine)
USING(glm)
USING(std)

Scene3D::Scene3D()
	: m_pSkyBox(nullptr)
	, m_pDefaultCamera(nullptr), m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pCharacterLayer(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();

	m_ObjListFileName = "mapObjects.json";
	m_LightListFileName = "lights.xml";
}

Scene3D::~Scene3D()
{
}

// Call instead of destructor to manage class internal data
void Scene3D::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pSkyBox);

	SafeDestroy(m_pUIManager);

	CScene::Destroy();
}

// Basic Update Function
void Scene3D::Update(const _float& dt)
{
	//if (nullptr != m_pSkyBox)
	//	CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	if (m_pUIManager)
		m_pUIManager->Update(dt);

	KeyCheck();

	CLightMaster::GetInstance()->SetLightInfo();

	CScene::Update(dt);
}

// Basic Render Function
void Scene3D::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

glm::vec3 Scene3D::GetCameraPos()
{
	if (nullptr != m_pDefaultCamera)
		return m_pDefaultCamera->GetCameraEye();

	return vec3(0.f);
}

CGameObject* Scene3D::GetTarget()
{
	CGameObject* pickedObj = nullptr;
	vector<CGameObject*> vecPicking;

	vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
	vec3 vDir = m_pInputDevice->GetMouseWorldCoord();
	for (int i = 0; i < m_vecLayer.size(); ++i)
	{
		if (!m_vecLayer[i]->GetEnable())
			continue;

		list<CGameObject*>* pObjList = m_vecLayer[i]->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = pObjList->begin(); iter != pObjList->end(); ++iter)
		{
			if ((*iter)->GetLock())
				continue;

			if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
				(*iter)->GetBoundingBox(),
				(*iter)->GetTransform(), vCameraPos, vDir))
			{
				vecPicking.push_back(*iter);
			}
		}
	}

	if (vecPicking.size() > 0)
	{
		_float distanceMin = FLT_MAX; _int index = 0;
		for (int i = 0; i < vecPicking.size(); ++i)
		{
			_float dis = distance(vCameraPos, vecPicking[i]->GetPosition());
			if (dis < distanceMin)
			{
				distanceMin = dis;
				index = i;
			}
		}

		pickedObj = vecPicking[index];
		pickedObj->SetSelected(true);
	}

	return pickedObj;
}

// Return current camera position
void Scene3D::KeyCheck()
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
void Scene3D::SetDefaultCameraSavedPosition(vec3 vPos, vec3 vRot, vec3 target)
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
void Scene3D::ResetDefaultCameraPos()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetCameraEye(m_vCameraSavedPos);
		m_pDefaultCamera->SetCameraRot(m_vCameraSavedRot);
		m_pDefaultCamera->SetCameraTarget(m_vCameraSavedTarget);
	}
}

// Add object
CGameObject* Scene3D::AddGameObject(eLAYERTAG tag, string meshID, glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale)
{
	CLayer* pLayer = GetLayer((_uint)tag);
	CGameObject* pGameObject = ObjectFactory::CreateGameObject(
		(_uint)SCENE_3D,
		pLayer->GetTag(),
		(_uint)OBJ_BACKGROUND,
		pLayer,
		meshID, vPos, vRot, vScale);

	return pGameObject;
}

// Save Objects
void Scene3D::SaveBackgroundObjects()
{
	vector<CJsonParser::sObjectData> vecObjects;
	for (int i = 0; i < m_vecLayer.size(); ++i)
	{
		if ((_uint)LAYER_CAMERA == m_vecLayer[i]->GetTag())
			continue;

		list<CGameObject*>* pObjList = m_vecLayer[i]->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = pObjList->begin(); iter != pObjList->end(); ++iter)
		{
			CGameObject* obj = *iter;
			CJsonParser::sObjectData data;
			data.ID = obj->GetMeshName();
			data.POSITION = obj->GetPosition();
			data.ROTATION = obj->GetRotation();
			data.SCALE = obj->GetScale();
			data.LOCK = obj->GetLock();
			data.SHOW = obj->GetEnable();
			data.ALPHA = obj->GetTransparency();
			data.LAYERTYPE = GetLayerNameByTag((eLAYERTAG)m_vecLayer[i]->GetTag());
			vecObjects.push_back(data);
		}
	}

	CJsonParser::sObjectData cameraData;
	if (nullptr != m_pDefaultCamera)
	{
		SetDefaultCameraSavedPosition(cameraData.POSITION, cameraData.ROTATION, cameraData.SCALE);
		cameraData.POSITION = m_pDefaultCamera->GetCameraEye();
		cameraData.ROTATION = m_pDefaultCamera->GetCameraRot();
		cameraData.SCALE = m_pDefaultCamera->GetCameraTarget();
	}

	CJsonParser::GetInstance()->SaveObjectList(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
}

// Load Objects
void Scene3D::LoadObjects()
{
	ResetAllLayers();

	CLayer* pLayer = GetLayer((_uint)LAYER_STATIC_OBJECT);
	CGameObject* pGameObject = nullptr;

	vector<CJsonParser::sObjectData> vecObjects;
	CJsonParser::sObjectData cameraData;
	CJsonParser::GetInstance()->LoadObjectList(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
	vector<CJsonParser::sObjectData>::iterator iter;
	for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
	{
		eLAYERTAG tag = GetLayerTagByName(iter->LAYERTYPE);
		pLayer = GetLayer((_uint)tag);
		pGameObject = ObjectFactory::CreateGameObject(
			(_uint)SCENE_3D,
			pLayer->GetTag(),
			(_uint)OBJ_BACKGROUND,
			pLayer,
			iter->ID, iter->POSITION, iter->ROTATION, iter->SCALE);

		if (nullptr != pGameObject)
		{
			pGameObject->SetLock(iter->LOCK);
			pGameObject->SetEnable(iter->SHOW);
			pGameObject->SetTransparency(iter->ALPHA);
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

// Empty all layers
void Scene3D::ResetAllLayers()
{
	for (_uint i = 0; i < m_vecLayer.size(); ++i)
	{
		if (i == (_uint)LAYER_CAMERA)
			continue;

		m_vecLayer[i]->RemoveAllGameObject();
	}
}

// Initialize
RESULT Scene3D::Ready(string dataPath)
{
	m_DataPath = dataPath;

	// Assimp test code
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("path", 
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
	}

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
		m_pUIManager->Ready(this);

	// SkyBox
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

// Initialize Layer and GameObjects
RESULT Scene3D::ReadyLayerAndGameObject()
{
	//Create.Layer
	AddLayer((_uint)LAYER_CAMERA);
	AddLayer((_uint)LAYER_STATIC_OBJECT);
	AddLayer((_uint)LAYER_INTERACTIVE_OBJECT);
	AddLayer((_uint)LAYER_CHARACTER);
	AddLayer((_uint)LAYER_ENEMY);
	AddLayer((_uint)LAYER_UI);
	AddLayer((_uint)LAYER_EVENT_OBJECT);
	m_pCharacterLayer = GetLayer((_uint)LAYER_CHARACTER);

	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_CAMERA);
	if (nullptr != pLayer)
	{
		m_pDefaultCamera = ObjectFactory::CreateCamera(
			(_uint)SCENE_3D, pLayer->GetTag(),
			(_uint)OBJ_CAMERA, pLayer,
			vec3(0.f), vec3(0.f), vec3(1.f), 0.6f, 0.1f, 1000.f);
	}

	//Create.BackgroundLayer 
	LoadObjects();

	return PK_NOERROR;
}

// Create an instance
Scene3D* Scene3D::Create(string dataPath)
{
	Scene3D* pInstance = new Scene3D();
	if (PK_NOERROR != pInstance->Ready(dataPath))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}