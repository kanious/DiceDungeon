#include "pch.h"
#include "GameManager.h"


USING(Engine)
SINGLETON_FUNCTION(CGameManager)

CGameManager::CGameManager()
//: m_pCurrentScene(nullptr), m_pRenderManager(nullptr)
{
	Awake();
}

CGameManager::~CGameManager()
{
}

void CGameManager::Awake()
{
	if (nullptr == m_pRenderManager)
		m_pRenderManager = CRenderManager::GetInstance();
}

void CGameManager::Update(const _float& dt)
{
	if (nullptr == m_pRenderManager ||
		nullptr == m_pCurrentScene)
		return;

	//m_pRenderManager->SetRenderOptions
	m_pCurrentScene->Update(dt);
}

void CGameManager::Render()
{
	if (nullptr == m_pRenderManager ||
		nullptr == m_pCurrentScene)
		return;

	m_pRenderManager->Render();
	//m_pCurrentScene->Render(); // 렌더는 렌더러에서만 하는걸로
}

void CGameManager::Destroy()
{
	DestroyInstance();
}

void CGameManager::SetCurrentScene(CScene* pScene)
{
	if (nullptr != m_pCurrentScene)
		m_pCurrentScene->Destroy();
	
	m_pCurrentScene = pScene;
}
