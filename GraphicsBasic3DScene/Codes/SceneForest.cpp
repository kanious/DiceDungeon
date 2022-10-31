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
#include "LightMaster.h"
#include "Light.h"

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
			{
				m_pTargetObject->SetSelected(false);
				m_pTargetObject = nullptr;
			}
			
			// Ray랑 오브젝트들의 Bounding Box와의 충돌 체크 해야댐
			if (nullptr != m_pBackgroundLayer)
			{
				vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
				vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

				list<CGameObject*>* listObj = m_pBackgroundLayer->GetObjectList();
				list<CGameObject*>::iterator iter;
				for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				{
					if ((*iter)->GetLock())
						continue;

					if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
						(*iter)->GetBoundingBox_AABB(),
						(*iter)->GetTransform(), vCameraPos, vDir))
					{
						m_pTargetObject = dynamic_cast<BGObject*>(*iter);
						m_pTargetObject->SetSelected(true);
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
			m_pTargetObject->SetSelected(false);
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
	CLightMaster::GetInstance()->SetLightInfo();

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
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
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
	MapEditorUIManager::GetInstance()->Ready(this, &m_pTargetObject, &m_pBackgroundLayer);

	// Light
	CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DefaultShader");
	_uint shaderID = 0;
	if (nullptr != shader)
		shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();

	CLight::cLightInfo* pInfo = new CLight::cLightInfo();
	pInfo->direction = vec4(1.0f, -1.f, 1.0f, 1.0f);
	pInfo->diffuse = vec4(1.0f, 1.0f, 1.0f, 0.2f);
	CLightMaster::GetInstance()->AddLight("Directional_Light", pInfo);

	pInfo = new CLight::cLightInfo();
	pInfo->param1.x = 1; //POINT_LIGHT
	pInfo->position = vec4(-20.f, 11.0f, 25.f, 1.0f);
	pInfo->specular = vec4(0.0f, 0.9f, 0.0f, 1.0f);
	pInfo->diffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	pInfo->atten = vec4(1.0f, 0.027f, 0.0028f, 5.f); // x = constant, y = linear, z = quadratic, w = cutoff range
	CLightMaster::GetInstance()->AddLight("Point_Light01", pInfo);

	//	distance	constant	linear		quadratic
	//	7			1.0			0.7			1.8
	//	13			1.0			0.35		0.44
	//	20			1.0			0.22		0.20
	//	32			1.0			0.14		0.07
	//	50			1.0			0.09		0.032
	//	65			1.0			0.07		0.017
	//	100			1.0			0.045		0.0075
	//	160			1.0			0.027		0.0028
	//	200			1.0			0.022		0.0019
	//	325			1.0			0.014		0.0007
	//	600			1.0			0.007		0.0002
	//	3250		1.0			0.0014		0.000007

	pInfo = new CLight::cLightInfo();
	pInfo->param1.x = 2; //SPOT_LIGHT
	pInfo->position = vec4(-10.f, 11.0f, -3.5f, 1.0f);
	pInfo->direction = vec4(0.0f, -1.0f, 0.0f, 0.0f);
	pInfo->specular = vec4(1.0f, 0.5f, 0.0f, 1.0f);
	pInfo->diffuse = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	pInfo->atten = vec4(1.0f, 0.014f, 0.0007f, 5.f); // x = constant, y = linear, z = quadratic, w = cutoff range
	pInfo->param1.y = 25.f; //inner angle(Theta)
	pInfo->param1.z = 35.f; //outer angle(Phi)
	CLightMaster::GetInstance()->AddLight("Spot_Light01", pInfo);


	//pInfo = new CLight::cLightInfo();
	//pInfo->param1.x = 1; //POINT_LIGHT
	////pInfo->position = vec4(-10.f, 5.0f/*10.9f*/, -3.5f, 1.0f);
	//pInfo->position = vec4(-11, 1.0f, -3.88f, 1.0f);
	//pInfo->specular = vec4(1.0f, 0.5f, 0.0f, 1.0f);
	//pInfo->diffuse = vec4(0.5f, 0.5f, 0.5f, 0.5f);
	//pInfo->atten = vec4(0.01f, 0.01f, 0.1f, 3.f); // x = constant, y = linear, z = quadratic, w = cutoff range
	//CLightMaster::GetInstance()->AddLight("Point_Light01", pInfo);

	CLightMaster::GetInstance()->SetUniformLocation(shaderID);

	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->SetShaderLocation(shaderID); 

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
		m_pBackgroundLayer = pLayer;
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
