#include "SceneDungeon.h"
#include "Function.h"
#include "glm\vec3.hpp"
#include "InputDevice.h"
#include "OpenGLDevice.h"
#include "DefaultCamera.h"
#include "JsonParser.h"
#include "Layer.h"
#include "SoundMaster.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Shader.h"
#include "LightMaster.h"
#include "BGObject.h"
#include "PhysicsObject.h"
#include "Define.h"
#include "UIManager.h"
#include "Enums.h"
#include "SkyBox.h"
#include "Player.h"
#include "Enemy.h"
#include "AnimationManager.h"
#include "AnimationData.h"
#include "ObjectFactory.h"
#include "TargetManager.h"
#include "PhysicsDefines.h"
#include "LuaBrain.h"
#include "TileMaster.h"
#include "CollisionMaster.h"
#include "EnemyManager.h"
#include "StateMachine.h"

#include <sstream>
#include <atlconv.h>
 

USING(Engine)
USING(glm)
USING(std)

SceneDungeon::SceneDungeon()
	: m_pSkyBox(nullptr)
	, m_pDefaultCamera(nullptr), m_vCameraSavedPos(vec3(0.f)), m_vCameraSavedRot(vec3(0.f)), m_vCameraSavedTarget(vec3(0.f))
	, m_pCharacterLayer(nullptr), m_pPFactory(nullptr), m_pPWorld(nullptr)
	, m_iTileIdx(0), m_bPicked(false)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance(); m_pUIManager->AddRefCnt();
	m_pAnimationManager = AnimationManager::GetInstance(); m_pAnimationManager->AddRefCnt();
	m_pTargetManager = TargetManager::GetInstance(); m_pTargetManager->AddRefCnt();
	m_pEnemyManager = EnemyManager::GetInstance(); m_pEnemyManager->AddRefCnt();

	m_ObjListFileName = "mapObjects.json";
	m_LightListFileName = "lights.xml";
	m_TileFileName = "tileData.json";
}

SceneDungeon::~SceneDungeon()
{
}

// Call instead of destructor to manage class internal data
void SceneDungeon::Destroy()
{
	TerminateThread(m_hPicking, 1);
	DeleteCriticalSection(&m_CSKey);

	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pUIManager);
	SafeDestroy(m_pPFactory);
	SafeDestroy(m_pPWorld);

	SafeDestroy(m_pSkyBox);
	SafeDestroy(m_pAnimationManager);
	SafeDestroy(m_pTargetManager);
	SafeDestroy(m_pEnemyManager);

	CScene::Destroy();
}

// Basic Update Function
void SceneDungeon::Update(const _float& dt)
{
	//if (nullptr != m_pSkyBox)
	//	CRenderer::GetInstance()->AddRenderObj(m_pSkyBox);

	if (nullptr != m_pPWorld)
		m_pPWorld->Update(dt);

	if (nullptr != m_pAnimationManager)
		m_pAnimationManager->Update(dt);

	if (nullptr != m_pEnemyManager)
		m_pEnemyManager->Update(dt);

	//if (nullptr != m_pTargetManager)
	//	m_pTargetManager->Update(dt);

	KeyCheck();
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

_bool SceneDungeon::GetPlayerPos(vec3& playerPos)
{
	if (nullptr == m_pPlayer)
		return false;

	playerPos = m_pPlayer->GetPosition();

	return true;
}

_bool SceneDungeon::GetPlayerTileIdx(_int& tileIdx)
{
	if (nullptr == m_pPlayer)
		return false;

	tileIdx = m_pPlayer->GetTileIdx();

	return true;
}

// Play sound if something collide
void SceneDungeon::CollisionSoundCallback()
{
	CSoundMaster::GetInstance()->PlaySound("Ball");
}

void SceneDungeon::ResetPicking()
{
	CTileMaster::GetInstance()->ClearAllLists();
	m_bPicked = false;
	UIManager::GetInstance()->SetPlayerMoveButtonWindow(false);
}

void SceneDungeon::MovePlayer()
{
	if (nullptr != m_pPlayer)
		m_pPlayer->StartMoving();
}

void SceneDungeon::HitPlayer()
{
	if (nullptr != m_pPlayer)
		m_pPlayer->Hit();
}

void SceneDungeon::StartEnemyTurn()
{
	m_pEnemyManager->StartEnemyTurn();
}

void SceneDungeon::StopEnemyTurn()
{
	m_pEnemyManager->StopEnemyTurn();
}

void SceneDungeon::GameOver()
{
	printf("Game Over\n");

	m_pEnemyManager->StopEnemyTurn();
}

void SceneDungeon::GameReset()
{
	printf("Game Reset\n");

	if (nullptr != m_pPlayer)
		m_pPlayer->Reset();

	CLayer* pLayer = GetLayer((_uint)LAYER_ENEMY);
	list<CGameObject*>* pObjList = pLayer->GetObjectList();
	list<CGameObject*>::iterator iter;
	for (iter = pObjList->begin(); iter != pObjList->end(); ++iter)
		dynamic_cast<Enemy*>(*iter)->Reset();

	ResetDefaultCameraPos();
}

// Check User input
void SceneDungeon::KeyCheck()
{
	if (m_pEnemyManager->GetEnemyTurn())
		return;

	if (m_pUIManager->GetCursorIsOnTheUI())
		return;

	static _bool isMouseLBClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isMouseLBClicked)
		{
			isMouseLBClicked = true;

			if (-1 != m_iTileIdx && m_iTileIdx != m_pPlayer->GetTileIdx())
			{
				EnterCriticalSection(&m_CSKey);
				m_bPicked = true;
				CTileMaster::GetInstance()->PathfindingStart(m_pPlayer->GetTileIdx(), m_iTileIdx);
				vec3 tilePos = CTileMaster::GetInstance()->GetTilePos(m_iTileIdx);
				UIManager::GetInstance()->SetPlayerMoveButtonWindow(true, tilePos);
				LeaveCriticalSection(&m_CSKey);
			}
		}
	}
	else
		isMouseLBClicked = false;

	static _bool isMouseRBClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_2))
	{
		if (!isMouseRBClicked)
		{
			isMouseRBClicked = true;
			ResetPicking();
		}
	}
	else
		isMouseRBClicked = false;

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

_ulong __stdcall SceneDungeon::TilePickingThread(LPVOID param)
{
	SceneDungeon* pScene = (SceneDungeon*)param;

	while (true)
	{
		Sleep(100);

		if (pScene->m_pEnemyManager->GetEnemyTurn())
		{
			if (-1 != pScene->m_iTileIdx)
			{
				EnterCriticalSection(&pScene->m_CSKey);
				CTileMaster::GetInstance()->TileEnable(pScene->m_iTileIdx, false);
				pScene->m_iTileIdx = -1;
				LeaveCriticalSection(&pScene->m_CSKey);
			}
			continue;
		}

		if (pScene->m_pUIManager->GetCursorIsOnTheUI())
			continue;

		if (!pScene->m_bPicked)
		{
			EnterCriticalSection(&pScene->m_CSKey);
			if (-1 != pScene->m_iTileIdx)
				CTileMaster::GetInstance()->TileEnable(pScene->m_iTileIdx, false);

			// Tile Picking
			vec3 vCameraPos = pScene->m_pDefaultCamera->GetCameraEye();
			vec3 vDir = pScene->m_pInputDevice->GetMouseWorldCoord();
			vec3 vDest = vec3(0.f);
			if (CCollisionMaster::GetInstance()->IntersectRayToVirtualPlane(1000.f, vCameraPos, vDir, vDest))
			{
				pScene->m_iTileIdx = CTileMaster::GetInstance()->FindPickedTile(vDest);
			}
			else
				pScene->m_iTileIdx = -1;

			if (-1 != pScene->m_iTileIdx)
				CTileMaster::GetInstance()->TileEnable(pScene->m_iTileIdx, true);

			LeaveCriticalSection(&pScene->m_CSKey);
		}
	}

	//if (m_pEnemyManager->GetEnemyTurn())
	//	return;

	//if (m_pUIManager->GetCursorIsOnTheUI())
	//	return;


	//if (!m_bPicked)
	//{
	//	if (-1 != m_iTileIdx)
	//		CTileMaster::GetInstance()->TileEnable(m_iTileIdx, false);
	//	m_iTileIdx = TilePicking();
	//	if (-1 != m_iTileIdx)
	//		CTileMaster::GetInstance()->TileEnable(m_iTileIdx, true);
	//}

	//EnterCriticalSection(&pScene->m_CSKey);
	//LeaveCriticalSection(&pScene->m_CSKey);

	return 0;
}

//_int SceneDungeon::TilePicking()
//{
//	vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
//	vec3 vDir = m_pInputDevice->GetMouseWorldCoord();
//	vec3 vDest = vec3(0.f);
//	if (CCollisionMaster::GetInstance()->IntersectRayToVirtualPlane(1000.f, vCameraPos, vDir, vDest))
//	{
//		return CTileMaster::GetInstance()->FindPickedTile(vDest);
//	}
//
//	return -1;
//}

// Initialize
RESULT SceneDungeon::Ready(string dataPath)
{
	m_DataPath = dataPath;

	// AnimationData
	//CAnimationData::GetInstance()->LoadAnimations(m_DataPath);

	// GameObjects
	RESULT result = PK_NOERROR;
	result = ReadyLayerAndCamera();
	if (PK_NOERROR != result)
		return result;

	// Physics
	ReadyPhysicsAndDice();

	// Light
	//CComponent* shader = CComponentMaster::GetInstance()->FindComponent("DeferredShader");
	//_uint shaderID = 0;
	//if (nullptr != shader)
	//	shaderID = dynamic_cast<CShader*>(shader)->GetShaderProgram();
	//CLightMaster::GetInstance()->SetShader(shaderID);
	//CLightMaster::GetInstance()->LoadLights(m_DataPath, m_LightListFileName);

	// Set Camera info to Shader
	if (nullptr != m_pDefaultCamera)
		m_pDefaultCamera->AddShaderLocation("DeferredShader");

	// UI
	if (nullptr != m_pUIManager)
		m_pUIManager->Ready(m_pPWorld, this);

	// TargetManager
	if (nullptr != m_pTargetManager)
		m_pTargetManager->Ready(this);

	// Lua
	CLuaBrain* pBrain = CLuaBrain::GetInstance();
	if (nullptr != pBrain)
	{
		pBrain->Ready(m_DataPath);
	}

	// Enemy Manager
	m_pEnemyManager->Ready(this);

	// Thread
	m_hPicking = CreateThread(NULL, 0, TilePickingThread, this, 0, nullptr);
	InitializeCriticalSection(&m_CSKey);

	//CSoundMaster::GetInstance()->PlaySound("Background");

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

// Initialize Layer and Camera
RESULT SceneDungeon::ReadyLayerAndCamera()
{
	//Create.Camera
	CLayer* pLayer = GetLayer((_uint)LAYER_INTERACTIVE_OBJECT);
	if (nullptr != pLayer)
	{
		vec3 vPos = vec3(0.f, 80.f, -60.f);
		vec3 vRot = vec3(0.f, 0.f, 0.f);
		vec3 vScale = vec3(1.f);
		SetDefaultCameraSavedPosition(vPos, vRot, vScale);
		m_pDefaultCamera = ObjectFactory::CreateCamera(
			(_uint)SCENE_3D, pLayer->GetTag(),
			(_uint)OBJ_CAMERA, pLayer,
			vPos, vRot, vScale, 0.6f, 0.1f, 1000.f);
	}

	//Create.Tiles
	LoadTiles();

	//Create.BackgroundLayer 
	LoadObjects();

	SortLayer();

	return PK_NOERROR;
}

// Load Objects from json file
void SceneDungeon::LoadObjects()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_STATIC_OBJECT);
	CGameObject* pGameObject = nullptr;

	vector<CJsonParser::sObjectData> vecObjects;
	CJsonParser::sObjectData cameraData;
	CJsonParser::GetInstance()->LoadObjectList(m_ObjListFileName, vecObjects, cameraData);
	vector<CJsonParser::sObjectData>::iterator iter;
	for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
	{
		eLAYERTAG tag = GetLayerTagByName(iter->LAYERTYPE);
		pLayer = GetLayer((_uint)tag);
		pGameObject = ObjectFactory::CreateGameObject(this,
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

		if (LAYER_CHARACTER == tag)
		{
			m_pPlayer = dynamic_cast<Player*>(pGameObject);
			StateMachine::SetPlayer(m_pPlayer);
		}
		else if (LAYER_ENEMY == tag)
			m_pEnemyManager->AddEnemy(dynamic_cast<Enemy*>(pGameObject));
	}
	vecObjects.clear();

	//if (nullptr != m_pDefaultCamera)
	//{
	//	SetDefaultCameraSavedPosition(cameraData.POSITION, cameraData.ROTATION, cameraData.SCALE);
	//	m_pDefaultCamera->SetCameraEye(cameraData.POSITION);
	//	m_pDefaultCamera->SetCameraRot(cameraData.ROTATION);
	//	m_pDefaultCamera->SetCameraTarget(cameraData.SCALE);
	//}
}

void SceneDungeon::LoadTiles()
{
	CLayer* pLayer = GetLayer((_uint)LAYER_TILE);
	CGameObject* pGameObject = nullptr;
	CTileMaster* pTileMaster = CTileMaster::GetInstance();

	vector<CJsonParser::sTileData> vecTiles;
	CJsonParser::GetInstance()->LoadTileData(m_TileFileName, vecTiles);
	vector<CJsonParser::sTileData>::iterator iter;
	for (iter = vecTiles.begin(); iter != vecTiles.end(); ++iter)
	{
		pGameObject = ObjectFactory::CreateGameObject(this,
			(_uint)SCENE_3D,
			pLayer->GetTag(),
			(_uint)OBJ_BACKGROUND,
			pLayer,
			"tile", iter->POSITION, vec3(0.f), vec3(0.025f));

		if (nullptr != pGameObject)
		{
			pGameObject->SetTransparency(false);
			pGameObject->SetEnable(false);
		}

		pTileMaster->AddTileNode(iter->ID, iter->POSITION, false, pGameObject);
	}
	for (iter = vecTiles.begin(); iter != vecTiles.end(); ++iter)
	{
		_uint tileIdx = iter->ID;
		for (int i = 0; i < 8; ++i)
			pTileMaster->SetNeighbor(tileIdx, i, iter->NEIGHBORS[i]);
	}

	vecTiles.clear();
}

// Initialize Physics World and prepare dice
void SceneDungeon::ReadyPhysicsAndDice()
{
	m_pPFactory = CPhysicsFactory::Create();
	m_pPWorld = m_pPFactory->CreateWorld(bind(&SceneDungeon::CollisionSoundCallback, this));
	if (nullptr == m_pPWorld)
		return;
	m_pPWorld->SetGravity(vec3(0.f, -35.f/*-9.81f*/, 0.f));
	m_pPWorld->SetCamera(m_pDefaultCamera->GetComponent("Camera"));

	CLayer* pLayer = GetLayer((_uint)LAYER_EVENT_OBJECT);
	CGameObject* pGameObject = nullptr;

	// Plane
	CRigidBodyDesc desc;
	desc.position = vec3(0.f);
	desc.rotation = vec3(0.f);
	desc.halfSize = vec3(1000.f, 1.f, 1000.f);
	desc.orientation = vec3(0.f);
	desc.forceAccum = vec3(0.f);
	iRigidBody* body = m_pPFactory->CreateRigidBody(desc, eShapeType::Plane);
	m_pPWorld->AddBody(body);

	for (int i = 0; i < 10; ++i)
	{
		pGameObject = ObjectFactory::CreateGameObject(this,
			(_uint)SCENE_3D, pLayer->GetTag(), (_uint)OBJ_BACKGROUND, pLayer,
			"dice", vec3(0.f), vec3(0.f), vec3(1.f));
		desc.position = vec3(0.f);
		desc.rotation = vec3(-8.f, -5.f, 7.f);
		desc.halfSize = vec3(1.f);
		desc.orientation = vec3(0.f);
		desc.forceAccum = vec3(0.f);
		body = m_pPFactory->CreateRigidBody(desc, eShapeType::Box);
		dynamic_cast<PhysicsObject*>(pGameObject)->SetRigidBody(body);
		m_pPWorld->AddBody(body);
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
