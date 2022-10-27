#include "Scene01.h"
#include "Layer.h"
#include "Player.h"
#include "TargetCamera.h"
#include "Component.h"
#include "Transform.h"

USING(Engine)

Scene01::Scene01()
{
	m_pTargetCamera = nullptr;
}

Scene01::~Scene01()
{
}

void Scene01::Update(const _float& dt)
{
	CScene::Update(dt);
}

void Scene01::Render()
{
}

void Scene01::Destroy()
{
	CScene::Destroy();
}

RESULT Scene01::Ready(eSCENETAG sceneTag)
{
	m_tag = (_uint)sceneTag;

	RESULT result = PK_NOERROR;

	CLayer* pLayer = nullptr;

	//Create.Layer
	pLayer = CLayer::Create((_uint)LAYER_OBJECT);
	if (nullptr != pLayer)
	{
		AddNewLayer((_uint)LAYER_OBJECT, pLayer);

		CGameObject* pGameObject = nullptr;
		//Create.Camera
		pGameObject = TargetCamera::Create(sceneTag, LAYER_CHARACTER, OBJ_PLAYER, nullptr);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer((_uint)LAYER_OBJECT, pGameObject);
			pGameObject->SetParentLayer(pLayer);
			m_pTargetCamera = dynamic_cast<TargetCamera*>(pGameObject);
		}
	}

	//Create.Layer
	pLayer = CLayer::Create((_uint)LAYER_CHARACTER);
	if (nullptr != pLayer)
	{
		AddNewLayer((_uint)LAYER_OBJECT, pLayer);

		CGameObject* pGameObject = nullptr;
		//Create.Player
		pGameObject = Player::Create(sceneTag, LAYER_CHARACTER, OBJ_PLAYER);
		if (nullptr != pGameObject)
		{
			AddGameObjectToLayer((_uint)LAYER_OBJECT, pGameObject);
			pGameObject->SetParentLayer(pLayer);
			if (nullptr != m_pTargetCamera)
			{
				m_pTargetCamera->SetTarget(
					dynamic_cast<CTransform*>(pGameObject->GetComponent((_uint)COMP_TRANSFORM)));
			}
		}
	}

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