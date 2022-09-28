#include "pch.h"
#include "ObjectManager.h"


USING(Engine)
SINGLETON_FUNCTION(CObjectManager)

CObjectManager::CObjectManager()
{
	Awake();
}

CObjectManager::~CObjectManager()
{
}

void CObjectManager::Awake()
{
	for (int i = 0; i < LAYER_END; ++i)
		m_listGameObjects[i].clear();
}

void CObjectManager::Update(const _float& dt)
{
	list<CGameObject*>::iterator iter;
	for (int i = 0; i < LAYER_END; ++i)
	{
		iter = m_listGameObjects[i].begin();
		for (iter; iter != m_listGameObjects[i].end(); ++iter)
		{
			if (nullptr != *iter)
			{
				if ((*iter)->GetEnable())
					(*iter)->Update(dt);
			}
		}
	}
}

void CObjectManager::Destroy()
{
}

void CObjectManager::DestroyManager()
{
	DestroyInstance();
}

void CObjectManager::AddGameObject(eLAYER tag, CGameObject* pInstance)
{
	if (nullptr != pInstance)
		m_listGameObjects[tag].push_back(pInstance);
}

void CObjectManager::RemoveGameObject(eLAYER tag, _wchar_t* uuid)
{
	list<CGameObject*>::iterator iter = m_listGameObjects[tag].begin();
	for (iter; iter != m_listGameObjects[tag].end(); ++iter)
	{
		if (nullptr != *iter)
		{
			if (uuid == (*iter)->GetUUID())
			{
				(*iter)->Destroy();
				m_listGameObjects[tag].erase(iter);
			}
		}
	}
}

void CObjectManager::RemoveAllObject()
{
	list<CGameObject*>::iterator iter;
	for (int i = 0; i < LAYER_END; ++i)
	{
		iter = m_listGameObjects[i].begin();
		for (iter; iter != m_listGameObjects[i].end(); ++iter)
		{
			if (nullptr != *iter)
				(*iter)->Destroy();
		}
		m_listGameObjects[i].clear();
	}
}
