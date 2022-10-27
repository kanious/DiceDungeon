#include "Scene01.h"
#include "Layer.h"
#include "Player.h"
#include "TargetCamera.h"
#include "Component.h"
#include "Transform.h"
#include "Enemy.h"
#include "Plane.h"
#include "ComponentManager.h"
#include "BattleManager.h"
#include "ConsoleManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "InputDevice.h"

USING(Engine)

Scene01::Scene01()
{
	m_pInputDevice = CInputDevice::GetInstance();
	m_pTargetCamera = nullptr;
}

Scene01::~Scene01()
{
}

void Scene01::Update(const _float& dt)
{
	static bool _isNDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_N))
	{
		if (!_isNDown)
		{
			_isNDown = true;
			RestartBattle();
		}
	}
	else
		_isNDown = false;

	CScene::Update(dt);
}

void Scene01::Render()
{
}

void Scene01::Destroy()
{
	CScene::Destroy();
}

void Scene01::RestartBattle()
{
	BattleManager::GetInstance()->SetNewGame();
	ConsoleManager::GetInstance()->ResetMessages();
	CLayer* pLayer = GetLayer((_uint)LAYER_CHARACTER);
	if (nullptr != pLayer)
	{
		pLayer->RemoveAllGameObject();

		AddPlayerToRandomCoord(pLayer);
		AddEnemyToRandomCoord(pLayer);
	}
}

void Scene01::AddPlayerToRandomCoord(CLayer* pLayer)
{
	vec3 vPos(0.f);
	vec3 vRot(0.f);
	vec3 vScale(0.5f);

	int randNum = rand() % 401 - 200;
	vPos.x = randNum * 0.1f;
	randNum = rand() % 401 - 200;
	vPos.z = randNum * 0.1f;
	randNum = rand() % 360;
	vRot.y = randNum;

	CGameObject* pGameObject = nullptr;
	pGameObject = Player::Create((eSCENETAG)m_tag, (eLAYERTAG)pLayer->GetTag(), OBJ_PLAYER, pLayer, vPos, vRot, vScale);
	if (nullptr != pGameObject)
	{
		AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
		if (nullptr != m_pTargetCamera)
		{
			m_pTargetCamera->SetTarget(
				dynamic_cast<CTransform*>(pGameObject->GetComponent((_uint)COMP_TRANSFORM)));
		}
		dynamic_cast<Player*>(pGameObject)->LookAt(vec3(0.f));
	}
}

void Scene01::AddEnemyToRandomCoord(CLayer* pLayer)
{
	vec3 vPos(0.f);
	vec3 vRot(0.f);
	vec3 vScale(0.5f);

	int randNum = rand() % 401 - 200;
	vPos.x = randNum * 0.1f;
	randNum = rand() % 401 - 200;
	vPos.z = randNum * 0.1f;
	randNum = rand() % 360;
	vRot.y = randNum;

	CGameObject* pGameObject = nullptr;
	pGameObject = Enemy::Create((eSCENETAG)m_tag, (eLAYERTAG)pLayer->GetTag(), OBJ_PLAYER, pLayer, vPos, vRot, vScale);
	if (nullptr != pGameObject)
	{
		AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
	}
	Enemy* pEnemy = dynamic_cast<Enemy*>(pGameObject);
	pEnemy->LookAt(vec3(0.f));
	BattleManager::GetInstance()->SetEnemy(pEnemy);
}

RESULT Scene01::Ready(eSCENETAG sceneTag)
{
	m_tag = (_uint)sceneTag;

	RESULT result = PK_NOERROR;
	result = ReadyComponent();
	if (PK_NOERROR != result)
		return result;

	result = ReadyLayer(sceneTag);
	if (PK_NOERROR != result)
		return result;
	
	return PK_NOERROR;
}

RESULT Scene01::ReadyLayer(eSCENETAG sceneTag)
{
	CLayer* pLayer = nullptr;
	CGameObject* pGameObject = nullptr;

	//Create.Layer
	pLayer = GetLayer((_uint)LAYER_BACKGROUND);
	if (nullptr != pLayer)
	{
		//Create.Plane
		pGameObject = Plane::Create(sceneTag, (eLAYERTAG)pLayer->GetTag(), OBJ_BACKGROUND,
			vec3(0.f, -0.1f, 0.f), vec3(0.f, 0.f, 0.f), vec3(40.f, 1.f, 40.f));
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
		}
	}

	//Create.Layer
	pLayer = GetLayer((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		//Create.Camera
		pGameObject = TargetCamera::Create(sceneTag, (eLAYERTAG)pLayer->GetTag(), OBJ_CAMERA, nullptr);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer(pLayer->GetTag(), pGameObject);
			m_pTargetCamera = dynamic_cast<TargetCamera*>(pGameObject);
		}
	}

	//Create.Layer
	pLayer = GetLayer((_uint)LAYER_CHARACTER);
	if (nullptr != pLayer)
	{
		AddPlayerToRandomCoord(pLayer);
		AddEnemyToRandomCoord(pLayer);
	}

	return PK_NOERROR;
}

RESULT Scene01::ReadyComponent()
{
	ComponentManager* pManager = ComponentManager::GetInstance();
	CComponent* pComponent = nullptr;

	//Create.Transform
	pComponent = CTransform::Create();
	if (nullptr != pComponent)
		pManager->AddTransform(pComponent);
	else
		return PK_TRANSFORM_CREATE_FAILED;

	//Create.Shader
	pComponent = CShader::Create("Shader\\BaseVertex.fx", "Shader\\BaseFragment.fx");
	if (nullptr != pComponent)
		pManager->AddShader(SHADER_BASE, pComponent);
	else
		return PK_SHADER_CREATE_FAILED;

	pComponent = CShader::Create("Shader\\MeshVertex.fx", "Shader\\MeshFragment.fx");
	if (nullptr != pComponent)
		pManager->AddShader(SHADER_MESH, pComponent);
	else
		return PK_SHADER_CREATE_FAILED;

	//Create.Mesh
	pComponent = CMesh::Create2("Assets\\tank.ply");
	if (nullptr != pComponent)
		pManager->AddMesh(MESH_TANK, pComponent);
	else
		return PK_MESH_CREATE_FAILED;

	pComponent = CMesh::Create3("Assets\\sphere.ply");
	if (nullptr != pComponent)
		pManager->AddMesh(MESH_SPHERE, pComponent);
	else
		return PK_MESH_CREATE_FAILED;

	//Create.Texture
	pComponent = CTexture::Create("Assets\\textures\\tank_1001_diff.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_TANK, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	pComponent = CTexture::Create("Assets\\textures\\tank_1001_diff_red.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_TANK_RED, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	pComponent = CTexture::Create("Assets\\textures\\grid.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_GRID, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	pComponent = CTexture::Create("Assets\\textures\\bullet.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_BULLET, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	pComponent = CTexture::Create("Assets\\textures\\boom.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_SPHERE, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	pComponent = CTexture::Create("Assets\\textures\\Barrel.png");
	if (nullptr != pComponent)
		pManager->AddTexture(TEX_BARREL, pComponent);
	else
		return PK_TEXTURE_CREATE_FAILED;

	return PK_NOERROR;
}

Scene01* Scene01::Create(eSCENETAG sceneTag)
{
	Scene01* pInstance = new Scene01;

	if (PK_NOERROR != pInstance->Ready(sceneTag))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}