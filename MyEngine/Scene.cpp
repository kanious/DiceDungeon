#include "pch.h"
#include "Scene.h"


USING(Engine)

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::Awake()
{
	if (nullptr == m_pObjectManager)
		m_pObjectManager = CObjectManager::GetInstance();
}

void CScene::Update(const _float& dt)
{
	if (nullptr != m_pObjectManager)
		m_pObjectManager->Update(dt);
}

void CScene::Render()
{
}

void CScene::Destroy()
{
	m_pObjectManager->Destroy();
}

void CScene::SetSceneTag(eSCENE tag)
{
	m_tag = tag;
}
