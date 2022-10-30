#include "SceneForest.h"
#include "glm\vec3.hpp"
#include "Define.h"
#include "Enums.h"

#include "XMLParser.h"
#include "Layer.h"
#include "GameObject.h"
#include "InputDevice.h"
#include "SoundMaster.h"
#include "SoundUIManager.h"
#include "MapEditorUIManager.h"
#include "CollisionMaster.h"

#include "BGObject.h"
#include "Camera.h"
#include "DefaultCamera.h"
#include <sstream>

USING(Engine)
USING(glm)
USING(std)

SceneForest::SceneForest()
	: m_pDefaultCamera(nullptr), m_pTargetObject(nullptr), m_pBackgroundLayer(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);

	m_DataPath = "Assets\\xmlData\\";
	m_SoundDataFileName = "SoundData.xml";
	m_ShaderDataFileName = "SceneForest_shaderdataList.xml";
	m_TextureDataFileName = "SceneForest_texturedataList.xml";
	m_MeshDataFileName = "SceneForest_meshdataList.xml";
	m_ObjListFileName = "SceneForest_mapObjects.xml";
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
			
			if (m_bMapEditorUIOpened)
			{
				if (MapEditorUIManager::GetInstance()->GetCursorIsOnTheUI())
					goto NextCheck;
			}

			if (nullptr != m_pTargetObject)
				m_pTargetObject = nullptr;
			
			// Ray랑 오브젝트들의 Bounding Box와의 충돌 체크 해야댐
			if (nullptr != m_pBackgroundLayer)
			{
				vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
				vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

				list<CGameObject*>* listObj = m_pBackgroundLayer->GetObjectList();
				list<CGameObject*>::iterator iter;
				for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				{
					if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
						(*iter)->GetBoundingBox_AABB(),
						(*iter)->GetTransform(), vCameraPos, vDir))
					{
						m_pTargetObject = dynamic_cast<BGObject*>(*iter);
						break;
					}
				}
			}

		}
	}
	else
		isMouseClicked = false;

	static _bool isDelDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_DELETE))
	{
		if (!isDelDown)
		{
			isDelDown = true;

			if (nullptr == m_pTargetObject)
				goto NextCheck;

			m_pTargetObject->SetDead(true);
			m_pTargetObject = nullptr;
		}
	}
	else
		isDelDown = false;

NextCheck:

	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_4))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y -= dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_5))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y += dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_3))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y = 0.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}
}

void SceneForest::Update(const _float& dt)
{
	KeyCheck(dt);

	if (nullptr != m_pDefaultCamera &&
 		nullptr != m_pTargetObject &&
		m_pDefaultCamera->GetMouseEnable())
	{
		if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_2))
		{
			if (m_bMapEditorUIOpened)
			{
				if (MapEditorUIManager::GetInstance()->GetCursorIsOnTheUI())
					goto NextCheck;
			}

			vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
			vec3 vDir = m_pInputDevice->GetMouseWorldCoord();
			vec3 vDest = vec3(0.f);
			if (CCollisionMaster::GetInstance()->IntersectRayToVirtualPlane(1000.f, vCameraPos, vDir, vDest))
			{
				vec3 vPos = m_pTargetObject->GetPosition();
				vDest.y = vPos.y;
				m_pTargetObject->SetPosition(vDest);
			}
		}

		vec2 vScroll = m_pInputDevice->GetMouseScrollDistance();
		_float fAngle = m_pTargetObject->GetRotationY();
		fAngle += vScroll.y * dt * 500.f;
		if (fAngle > 360.f)
			fAngle -= 360.f;
		if (fAngle < 0)
			fAngle += 360.f;
		m_pTargetObject->SetRotationY(fAngle);
	}
NextCheck:

	CScene::Update(dt);
}

void SceneForest::Render()
{
	//SoundUIManager::GetInstance()->RenderUI();
	MapEditorUIManager::GetInstance()->RenderUI();
}

void SceneForest::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CScene::Destroy();
}

RESULT SceneForest::Ready()
{
	RESULT result = PK_NOERROR;
	result = ReadyComponent();
	if (PK_NOERROR != result)
		return result;

	result = ReadyLayerAndGameObject();
	if (PK_NOERROR != result)
		return result;

	stringstream ss;
	ss << m_DataPath << m_SoundDataFileName;
	CXMLParser::GetInstance()->LoadSoundData(ss.str());

	//SoundUIManager::GetInstance()->Ready();
	MapEditorUIManager::GetInstance()->Ready(this, &m_pTargetObject);

	return PK_NOERROR;
}

RESULT SceneForest::ReadyComponent()
{
	//Create.Shader
	stringstream ss;
	ss << m_DataPath << m_ShaderDataFileName;
	CXMLParser::GetInstance()->LoadShaderData(ss.str());

	//Create.Texture
	ss.str("");
	ss << m_DataPath << m_TextureDataFileName;
	CXMLParser::GetInstance()->LoadTextureData(ss.str());

	//Create.Mesh
	ss.str("");
	ss << m_DataPath << m_MeshDataFileName;
	CXMLParser::GetInstance()->LoadMeshData(ss.str());

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
			data.vPos = pObj->GetPosition();
			data.vRot = pObj->GetRotation();
			data.vScale = pObj->GetScale();
			vecObjects.push_back(data);
		}

		CXMLParser::sObjectData cameraData;
		if (nullptr != m_pDefaultCamera)
		{
			cameraData.vPos = m_pDefaultCamera->GetCameraEye();
			cameraData.vRot = m_pDefaultCamera->GetCameraRot();
			cameraData.vScale = m_pDefaultCamera->GetCameraTarget();
		}

		stringstream ss;
		ss << m_DataPath << m_ObjListFileName;
		CXMLParser::GetInstance()->SaveMapObjectData(vecObjects, cameraData, ss.str());
	}
}

void SceneForest::LoadBackgroundObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	CGameObject* pGameObject = nullptr;

	if (nullptr != pLayer)
	{
		pLayer->RemoveAllGameObject();
		stringstream ss;
		ss << m_DataPath << m_ObjListFileName;
		vector<CXMLParser::sObjectData> vecObjects;
		CXMLParser::sObjectData cameraData;
		CXMLParser::GetInstance()->LoadMapObjectData(vecObjects, cameraData, ss.str());
		vector<CXMLParser::sObjectData>::iterator iter;
		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = BGObject::Create((_uint)SCENE_FOREST, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->vPos, iter->vRot, iter->vScale);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
		}
		vecObjects.clear();

		if (nullptr != m_pDefaultCamera)
		{
			m_pDefaultCamera->SetCameraEye(cameraData.vPos);
			m_pDefaultCamera->SetCameraRot(cameraData.vRot);
			m_pDefaultCamera->SetCameraTarget(cameraData.vScale);
		}
	}
}
