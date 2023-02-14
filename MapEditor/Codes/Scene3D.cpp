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
#include "Enums.h"
#include "SkyBox.h"
#include "Renderer.h"
#include "Player.h"
#include "AnimationManager.h"
#include "AnimationData.h"

#include <sstream>
#include <atlconv.h>


USING(Engine)
USING(glm)
USING(std)

Scene3D::Scene3D()
	: m_pSkyBox(nullptr)
	, m_pDefaultCamera(nullptr), m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pObjLayer(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();
	m_pAnimationManager = AnimationManager::GetInstance(); m_pAnimationManager->AddRefCnt();

	m_ObjListFileName = "mapObjects.json";
	m_LightListFileName = "lights.xml";
}

Scene3D::~Scene3D()
{
}

void Scene3D::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pSkyBox);

	SafeDestroy(m_pUIManager);
	SafeDestroy(m_pAnimationManager);

	CScene::Destroy();
}

void Scene3D::Update(const _float& dt)
{
	//if (nullptr != m_pSkyBox)
	//	CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	if (m_pUIManager)
		m_pUIManager->Update(dt);

	KeyCheck();

	CLightMaster::GetInstance()->SetLightInfo();
	if (nullptr != m_pAnimationManager)
		m_pAnimationManager->Update(dt);

	CScene::Update(dt);
}

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

	/*
	static _bool isMouseClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isMouseClicked)
		{
			isMouseClicked = true;

			if (nullptr != m_pObjLayer)
			{
				vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
				vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

				vector<CGameObject*> vecPicking;
				list<CGameObject*>* listObj = m_pObjLayer->GetObjectList();
				list<CGameObject*>::iterator iter;
				for (iter = listObj->begin(); iter != listObj->end(); ++iter)
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

					dynamic_cast<Player*>(vecPicking[index])->SetTarget();
				}
				else
					AnimationManager::GetInstance()->SetTargetAnimator(nullptr);
			}
		}
	}
	else
		isMouseClicked = false;
	*/
}

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

void Scene3D::ResetDefaultCameraPos()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetCameraEye(m_vCameraSavedPos);
		m_pDefaultCamera->SetCameraRot(m_vCameraSavedRot);
		m_pDefaultCamera->SetCameraTarget(m_vCameraSavedTarget);
	}
}

RESULT Scene3D::Ready(string dataPath)
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
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	// Set Camera info to Shader
	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->AddShaderLocation("DefaultShader");

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

RESULT Scene3D::ReadyLayerAndGameObject()
{
	//Create.Layer
	AddLayer((_uint)LAYER_BACKGROUND_OBJECT);
	AddLayer((_uint)LAYER_STATIC_OBJECT);
	AddLayer((_uint)LAYER_INTERACTIVE_OBJECT);
	AddLayer((_uint)LAYER_CHARACTER);
	AddLayer((_uint)LAYER_ENEMY);
	AddLayer((_uint)LAYER_UI);
	AddLayer((_uint)LAYER_EVENT_OBJECT);

	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_INTERACTIVE_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 0.f, 0.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		CGameObject* pGameObject = DefaultCamera::Create((_uint)SCENE_3D, pLayer->GetTag(), (_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			m_pDefaultCamera = dynamic_cast<DefaultCamera*>(pGameObject);
		}
	}

	//Create.BackgroundLayer 
	LoadBackgroundObjects();

	//Create.Player/AIs
	//AddCharacters();

	return PK_NOERROR;
}

void Scene3D::SaveBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND_OBJECT);
	if (nullptr != pLayer)
	{
		vector<CJsonParser::sObjectData> vecObjects;

		list<CGameObject*>* pObjList = pLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = pObjList->begin(); iter != pObjList->end(); ++iter)
		{
			BGObject* pObj = dynamic_cast<BGObject*>(*iter);
			CJsonParser::sObjectData data;
			data.ID = pObj->GetMeshID();
			data.POSITION = pObj->GetPosition();
			data.ROTATION = pObj->GetRotation();
			data.SCALE = pObj->GetScale();
			data.LOCK = pObj->GetLock();
			data.LAYERTYPE = pObj->GetTexName();
			vecObjects.push_back(data);
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
}

void Scene3D::LoadBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND_OBJECT);
	CGameObject* pGameObject = nullptr;

	if (nullptr != pLayer)
	{
		pLayer->RemoveAllGameObject();
		vector<CJsonParser::sObjectData> vecObjects;
		CJsonParser::sObjectData cameraData;
		CJsonParser::GetInstance()->LoadObjectList(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
		vector<CJsonParser::sObjectData>::iterator iter;
		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = BGObject::Create((_uint)SCENE_3D, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer,
				iter->ID, iter->POSITION, iter->ROTATION, iter->SCALE);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			dynamic_cast<BGObject*>(pGameObject)->SetLock(iter->LOCK);
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

void Scene3D::AddCharacters()
{
	CGameObject* pGameObject = nullptr;
	m_pObjLayer = GetLayer((_uint)LAYER_CHARACTER);

	if (nullptr == m_pObjLayer)
		return;

	//Json.Parser
	vector<CJsonParser::sCharacterData> vecCharacters;
	CJsonParser::GetInstance()->LoadCharacterList(m_DataPath, "PlayerList.json", vecCharacters);
	for (int i = 0; i < vecCharacters.size(); ++i)
	{
		CJsonParser::sCharacterData data = vecCharacters[i];

		pGameObject = Player::Create((_uint)SCENE_3D, m_pObjLayer->GetTag(), (_uint)OBJ_CHARACTER, m_pObjLayer
			, data.MESHID, data.POSITION, data.ROTATION, data.SCALE, (eAnimType)data.ANIMTYPE, (eEaseType)data.EASETYPE
			, data.ANIMRANDOM);
		if (nullptr != pGameObject)
			AddGameObjectToLayer(m_pObjLayer->GetTag(), pGameObject);
	}
}

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