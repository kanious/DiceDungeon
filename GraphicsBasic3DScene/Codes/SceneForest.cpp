#include "SceneForest.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "glm\vec3.hpp"
#include "Define.h"
#include "Enums.h"

#include "XMLParser.h"
#include "Camera.h"
#include "Transform.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Layer.h"
#include "GameObject.h"
#include "InputDevice.h"
#include "SoundMaster.h"
#include "SoundUIManager.h"
#include "MapEditorUIManager.h"
#include "CollisionMaster.h"

#include "Tree.h"
#include "DefaultCamera.h"

USING(Engine)
USING(glm)
USING(std)

SceneForest::SceneForest()
	: m_pDefaultCamera(nullptr), m_pTargetObject(nullptr), m_pBackgroundLayer(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);
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
				vec2 vMousePos = m_pInputDevice->GetMousePos();
				if (vMousePos.x < 400)
					goto NextCheck;
			}

			if (nullptr != m_pTargetObject)
				m_pTargetObject = nullptr;
			else
			{
				// Ray랑 오브젝트들의 Bounding Box와의 충돌 체크 해야댐
				if (nullptr != m_pBackgroundLayer)
				{
					vec3 vCameraPos = m_pDefaultCamera->GetCurrentPos();
					vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

					list<CGameObject*>* listObj = m_pBackgroundLayer->GetObjectList();
					list<CGameObject*>::iterator iter;
					for (iter = listObj->begin(); iter != listObj->end(); ++iter)
					{
						if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
							(*iter)->GetBoundingBox_AABB(),
							(*iter)->GetPosition(),
							(*iter)->GetScale(), vCameraPos, vDir))
						{
 							m_pTargetObject = dynamic_cast<Tree*>(*iter);
							break;
						}
					}
				}
			}
		}
	}
	else
		isMouseClicked = false;

NextCheck:

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_KP_8))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y += dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_KP_2))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y -= dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_KP_0))
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
		if (m_pInputDevice->IsMousePressing(GLFW_MOUSE_BUTTON_2))
		{
			if (m_bMapEditorUIOpened)
			{
				vec2 vMousePos = m_pInputDevice->GetMousePos();
				if (vMousePos.x < 400)
					goto NextCheck;
			}

			vec3 vCameraPos = m_pDefaultCamera->GetCurrentPos();
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

	CXMLParser::GetInstance()->LoadSoundData("Assets\\xmlData\\SoundData.xml");

	//SoundUIManager::GetInstance()->Ready();
	MapEditorUIManager::GetInstance()->Ready(this);

	return PK_NOERROR;
}

RESULT SceneForest::ReadyComponent()
{
	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Create.Camera
	pComponent = CCamera::Create();
	if (nullptr != pComponent)
		pMaster->AddNewComponent("Camera", pComponent);
	else
		return PK_CAMERA_CREATE_FAILED;

	//Create.Transform
	pComponent = CTransform::Create();
	if (nullptr != pComponent)
		pMaster->AddNewComponent("Transform", pComponent);
	else
		return PK_TRANSFORM_CREATE_FAILED;

	//Create.Mesh
	vector<CXMLParser::sMeshdata> vecMesh;
	CXMLParser::GetInstance()->LoadMeshData(vecMesh, "Assets\\xmlData\\SceneForest_meshdataList.xml");
	vector<CXMLParser::sMeshdata>::iterator iter;
	for (iter = vecMesh.begin(); iter != vecMesh.end(); ++iter)
	{
		pComponent = CMesh::Create(iter->PATH, iter->FILENAME, (ModelType)iter->TYPE,
			iter->SHADER_ID, iter->VERTEXSHADER_PATH, iter->FRAGMENTSHADER_PATH);
		if (nullptr != pComponent)
		{
			pMaster->AddNewComponent(iter->ID, pComponent);
			pMaster->AddNewMeshInfo(iter->ID, iter->INITSIZE);
		}
	}
	vecMesh.clear();

	return PK_NOERROR;
}

RESULT SceneForest::ReadyLayerAndGameObject()
{
	CLayer* pLayer = nullptr;
	CGameObject* pGameObject = nullptr;

	//Create.BackgroundLayer
	pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	if (nullptr != pLayer)
	{
		m_pBackgroundLayer = pLayer;
		//Create.Objects
		vector<CXMLParser::sObjectData> vecObjects;
		CXMLParser::GetInstance()->LoadMapObjectData(vecObjects, "Assets\\xmlData\\SceneForest_mapObjects.xml");
		vector<CXMLParser::sObjectData>::iterator iter;
		for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
		{
			pGameObject = Tree::Create((_uint)SCENE_FOREST, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->vPos, iter->vRot, iter->vScale);
			if (nullptr == pGameObject)
				continue;
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
		}
		vecObjects.clear();
	}

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
