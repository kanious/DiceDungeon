#include "Scene3D.h"
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
#include "Define.h"
#include "UIManager.h"
#include "Enums.h"
#include "SkyBox.h"
#include "Renderer.h"
#include "Player.h"
#include "NetworkManager.h"
#include "InputInfo.h"
#include "Transform.h"
#include "proto\GameState.pb.h"


USING(Engine)
USING(glm)
USING(std)

Scene3D::Scene3D()
	: m_pDefaultCamera(nullptr), m_pObjLayer(nullptr), m_pSkyBox(nullptr)
	, m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pInputInfo(nullptr), m_iIndex(-1)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();
	m_pNetworkManager = NetworkManager::GetInstance(); m_pNetworkManager->AddRefCnt();

	for (int i = 0; i < 4; ++i)
	{
		m_pPlayer[i] = nullptr;
		m_pBullet[i] = nullptr;
	}

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));//
	stringstream ss;
	ss << str << "\\..\\";

	m_DataPath = ss.str();
	m_ObjListFileName = "UDPGame_mapObjects.xml";
	m_LightListFileName = "UDPGame_lights.xml";
}

Scene3D::~Scene3D()
{
}

void Scene3D::KeyCheck(const _float& dt)
{
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

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_W))
		m_pInputInfo->forward = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_A))
		m_pInputInfo->left = true;
	
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_S))
		m_pInputInfo->backward = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_D))
		m_pInputInfo->right = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_Q))
		m_pInputInfo->turnLeft = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_E))
		m_pInputInfo->turnRight = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_F))
		m_pInputInfo->fire = true;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_R))
		m_pInputInfo->respawn = true;

	if (-1 != m_iIndex)
	{
		if (m_pInputInfo->forward)
		{
			vec3 vDir = m_pPlayer[m_iIndex]->GetTransform()->GetLookVector();
			m_pPlayer[m_iIndex]->GetTransform()->AddPosition(-vDir * dt * 10.f);
		}
		if (m_pInputInfo->left)
		{
			vec3 vDir = m_pPlayer[m_iIndex]->GetTransform()->GetRightVector();
			m_pPlayer[m_iIndex]->GetTransform()->AddPosition(-vDir * dt * 10.f);
		}
		if (m_pInputInfo->backward)
		{
			vec3 vDir = m_pPlayer[m_iIndex]->GetTransform()->GetLookVector();
			m_pPlayer[m_iIndex]->GetTransform()->AddPosition(vDir * dt * 10.f);
		}
		if (m_pInputInfo->right)
		{
			vec3 vDir = m_pPlayer[m_iIndex]->GetTransform()->GetRightVector();
			m_pPlayer[m_iIndex]->GetTransform()->AddPosition(vDir * dt * 10.f);
		}
		if (m_pInputInfo->turnLeft)
		{
			m_pPlayer[m_iIndex]->GetTransform()->AddRotationY(dt * 50.f);
		}
		if (m_pInputInfo->turnRight)
		{
			m_pPlayer[m_iIndex]->GetTransform()->AddRotationY(-dt * 50.f);
		}
	}
}

void Scene3D::ResetInput()
{
	m_pInputInfo->Reset();
}

void Scene3D::SetGameState(GameState& packet)
{
	m_iIndex = packet.clientid();
	for (int i = 0; i < 4; ++i)
	{
		Object* obj = packet.mutable_players(i);
		vec3 vPos = vec3(obj->posx()
			, obj->posy()
			, obj->posz());
		_float fY = obj->roty();
		_int state = obj->state();

		//if (i == m_iIndex)
		//{
		//	if (0 == state)
		//		m_pPlayer[i]->SetEnable(false);
		//	else
		//		m_pPlayer[i]->SetEnable(true);

		//	if (m_pNetworkManager->GetTickNumber() > obj->tick_number())
		//		continue;

		//	vec3 vClientPos = m_pPlayer[m_iIndex]->GetPosition();
		//	_float dist = distance(vPos, vClientPos);
		//	if (dist > 0.1f)
		//	{
		//		m_pPlayer[i]->SetPosition(vPos);
		//		std::cout << "fixed .";
		//	}
		//	else
		//		std::cout<< "passed .";
		//}
		//else
		//{
			if (0 == state)
				m_pPlayer[i]->SetEnable(false);
			else
			{
				m_pPlayer[i]->SetEnable(true);
				m_pPlayer[i]->SetPosition(vPos);
				m_pPlayer[i]->SetRotationY(fY);
			}
		//}

		Object* bullet = packet.mutable_bullets(i);
		vPos = vec3(bullet->posx()
			, bullet->posy()
			, bullet->posz());
		fY = bullet->roty();
		state = bullet->state();

		if (0 == state)
			m_pBullet[i]->SetEnable(false);
		else
		{
			m_pBullet[i]->SetEnable(true);
			m_pBullet[i]->SetPosition(vPos);
			m_pBullet[i]->SetRotationY(fY);
		}
	}
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

void Scene3D::Update(const _float& dt)
{
	if (nullptr != m_pSkyBox)
		CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	CLightMaster::GetInstance()->SetLightInfo();

	KeyCheck(dt);

	CScene::Update(dt);
}

void Scene3D::Render()
{
	if (nullptr != m_pUIManager)
		m_pUIManager->RenderUI();
}

void Scene3D::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pUIManager);
	SafeDestroy(m_pSkyBox);
	SafeDestroy(m_pNetworkManager);

	CScene::Destroy();
}

RESULT Scene3D::Ready()
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
	{
		m_pDefaultCamera->AddShaderLocation("DefaultShader");
	}

	if (nullptr != m_pUIManager)
		m_pUIManager->Ready();

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

	m_pInputInfo = m_pNetworkManager->GetUserInput();

	return PK_NOERROR;
}

RESULT Scene3D::ReadyLayerAndGameObject()
{
	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_OBJECT);
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

	return PK_NOERROR;
}

Scene3D* Scene3D::Create()
{
	Scene3D* pInstance = new Scene3D();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

void Scene3D::LoadBackgroundObjects()
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
			pGameObject = BGObject::Create((_uint)SCENE_3D, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer, iter->ID,
				iter->POSITION, iter->ROTATION, iter->SCALE);
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

		for (int i = 0; i < 4; ++i)
		{
			pGameObject = Player::Create((_uint)SCENE_3D, m_pObjLayer->GetTag(), (_uint)OBJ_PLAYER, m_pObjLayer
				, "Sphere", vec3(0.f), vec3(0.f), vec3(0.5f));
			if (nullptr != pGameObject)
			{
				AddGameObjectToLayer(m_pObjLayer->GetTag(), pGameObject);
				m_pPlayer[i] = dynamic_cast<Player*>(pGameObject);
				m_pPlayer[i]->SetEnable(false);
			}

			pGameObject = Player::Create((_uint)SCENE_3D, m_pObjLayer->GetTag(), (_uint)OBJ_PLAYER, m_pObjLayer
				, "Sphere", vec3(0.f), vec3(0.f), vec3(0.1f));
			if (nullptr != pGameObject)
			{
				AddGameObjectToLayer(m_pObjLayer->GetTag(), pGameObject);
				m_pBullet[i] = dynamic_cast<Player*>(pGameObject);
				m_pBullet[i]->SetEnable(false);
			}
		}
	}
}