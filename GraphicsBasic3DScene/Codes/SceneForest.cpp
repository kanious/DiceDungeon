#include "SceneForest.h"
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
#include "SkyBox.h"
#include "Renderer.h"

#include "BGObject.h"
#include "Camera.h"
#include "DefaultCamera.h"
#include <sstream>
#include <atlconv.h>

USING(Engine)
USING(glm)
USING(std)

SceneForest::SceneForest()
	: m_pDefaultCamera(nullptr), m_pBackgroundLayer(nullptr), m_pSkyBox(nullptr)
	, m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
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
	m_ObjListFileName = "UDPGame_mapObjects.xml";
	m_LightListFileName = "UDPGame_lights.xml";
}

SceneForest::~SceneForest()
{
}

void SceneForest::KeyCheck(const _float& dt)
{
	static _bool isMouseClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isMouseClicked)
		{
			isMouseClicked = true;

			if (nullptr == m_pUIManager)
				goto NextCheck;

			if (m_pUIManager->GetUIOpened(UIManager::UI_MAP_EDITOR_WINDOW) ||
				m_pUIManager->GetUIOpened(UIManager::UI_SYSTEM_MENU_WINDOW))
				goto NextCheck;

			if (m_pUIManager->GetCursorIsOnTheUI())
				goto NextCheck;

			if (nullptr != m_pBackgroundLayer)
			{
				vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
				vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

				vector<CGameObject*> vecPicking;
				list<CGameObject*>* listObj = m_pBackgroundLayer->GetObjectList();
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

					dynamic_cast<BGObject*>(vecPicking[index])->PlaySound();
				}
			}
		}
	}
	else
		isMouseClicked = false;

NextCheck:
	// Call System Menu Window
	static _bool isEscapeDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_ESCAPE))
	{
		if (!isEscapeDown)
		{
			isEscapeDown = true;

			if (nullptr != m_pDefaultCamera && !m_pDefaultCamera->GetMouseEnable())
				m_pDefaultCamera->SetMouseEnable(true);

			m_pUIManager->SetUI(UIManager::UI_SYSTEM_MENU_WINDOW);
		}
	}
	else
		isEscapeDown = false;

	// Reset Camera Pos
	static _bool isF1Down = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F1))
	{
		if (!isF1Down)
		{
			isF1Down = true;

			ResetDefaultCameraPos();
		}
	}
	else
		isF1Down = false;
}

void SceneForest::Update(const _float& dt)
{
	if (nullptr != m_pSkyBox)
		CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	CLightMaster::GetInstance()->SetLightInfo();

	if (nullptr != m_pUIManager)
		m_pUIManager->Update(dt);

	KeyCheck(dt);

	CScene::Update(dt);
}

void SceneForest::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

void SceneForest::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pUIManager);
	SafeDestroy(m_pSkyBox);

	CScene::Destroy();
}

void SceneForest::SetDefaultCameraSavedPosition(vec3 vPos, vec3 vRot, vec3 target)
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

void SceneForest::ResetDefaultCameraPos()
{
	if (nullptr != m_pDefaultCamera)
	{
		m_pDefaultCamera->SetCameraEye(m_vCameraSavedPos);
		m_pDefaultCamera->SetCameraRot(m_vCameraSavedRot);
		m_pDefaultCamera->SetCameraTarget(m_vCameraSavedTarget);
	}
}

RESULT SceneForest::Ready()
{
	RESULT result = PK_NOERROR;

	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	CSoundMaster::GetInstance()->PlaySound("peaceful_night");
	CSoundMaster::GetInstance()->PlaySound("chirp");
	CSoundMaster::GetInstance()->PlaySound("frames");

	if (nullptr != m_pUIManager)
		m_pUIManager->Ready(this);

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	CLightMaster::GetInstance()->SetShader(shaderID);
	CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID);

	//if (nullptr != m_pDefaultCamera)
	//{
	//	m_pDefaultCamera->SetShaderLocation(shaderID);
	//	shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	//	if (nullptr != shader)
	//		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	//	m_pDefaultCamera->SetShaderLocation2(shaderID);
	//}

	if (nullptr == m_pSkyBox)
	{
		stringstream ss, ss2;
		ss << m_DataPath << "Assets\\Texture\\SkyBox\\";

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

RESULT SceneForest::ReadyLayerAndGameObject()
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

SceneForest* SceneForest::Create()
{
	SceneForest* pInstance = new SceneForest();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void SceneForest::SaveBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	if (nullptr != pLayer)
	{
		vector<CXMLParser::sObjectData> vecObjects;

		list<CGameObject*>* pObjList = pLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = pObjList->begin(); iter != pObjList->end(); ++iter)
		{
			BGObject* pObj = dynamic_cast<BGObject*>(*iter);
			CXMLParser::sObjectData data;
			data.ID = pObj->GetMeshID();
			data.POSITION = pObj->GetPosition();
			data.ROTATION = pObj->GetRotation();
			data.SCALE = pObj->GetScale();
			data.SOUNDTAG = pObj->GetSoundTag();
			data.LOCK = pObj->GetLock();
			vecObjects.push_back(data);
		}

		CXMLParser::sObjectData cameraData;
		if (nullptr != m_pDefaultCamera)
		{
			SetDefaultCameraSavedPosition(cameraData.POSITION, cameraData.ROTATION, cameraData.SCALE);
			cameraData.POSITION = m_pDefaultCamera->GetCameraEye();
			cameraData.ROTATION = m_pDefaultCamera->GetCameraRot();
			cameraData.SCALE = m_pDefaultCamera->GetCameraTarget();
		}

		CXMLParser::GetInstance()->SaveMapObjectData(m_DataPath, m_ObjListFileName, vecObjects, cameraData);
	}
}

void SceneForest::LoadBackgroundObjects()
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
			pGameObject = BGObject::Create((_uint)SCENE_FOREST, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->POSITION, iter->ROTATION, iter->SCALE, 0);
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