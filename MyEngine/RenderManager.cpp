#include "pch.h"
#include "RenderManager.h"


USING(Engine)
SINGLETON_FUNCTION(CRenderManager)

CRenderManager::CRenderManager()
{
	Awake();
}

CRenderManager::~CRenderManager()
{
}

void CRenderManager::Awake()
{
}

void CRenderManager::Render()
{
	vector<CGameObject*>::iterator iter = m_vecRenderObj.begin();
	for (iter; iter != m_vecRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
}

void CRenderManager::Destroy()
{
	DestroyInstance();
}

void CRenderManager::AddRenderObj(CGameObject* obj)
{
	if (nullptr == obj)
		return;

	m_vecRenderObj.push_back(obj);
}

void CRenderManager::RemoveRenderObj()
{
	m_vecRenderObj.clear();
}
