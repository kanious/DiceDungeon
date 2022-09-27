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
	for (int i = 0; i < SCENE_END; ++i)
	{
		LAYER_MAP* pMap = new LAYER_MAP;
		for (int j = 0; j < LAYER_END; ++j)
		{
			CLayer* pLayer = CLayer::Create((eLAYER)j);
			pMap->insert(LAYER_MAP::value_type((eLAYER)j, pLayer));
		}
		m_mapLayer.insert(SCENE_MAP::value_type((eSCENE)i, pMap));
	}
}

void CObjectManager::Destroy()
{
	for (int i = 0; i < SCENE_END; ++i)
	{
		SCENE_MAP::const_iterator iter = m_mapLayer.find((eSCENE)i);
		if (iter != m_mapLayer.end())
		{
			LAYER_MAP* pMap = iter->second;
			for (int j = 0; j < LAYER_END; ++j)
			{
				LAYER_MAP::const_iterator iter2 = pMap->find((eLAYER)j);
				if (iter2 != pMap->end())
				{
					CLayer* pLayer = iter2->second;
					if (nullptr != pLayer)
						pLayer->Destroy();
				}
			}
			pMap->clear();
		}
		m_mapLayer.clear();
	}

	DestroyInstance();
	// Destroy에서 자기 파괴 할껀지 소멸자를 public 하게 만들건지 결정
	// Destroy에서 자기 파괴 하는게 나을 꺼 같긴 해.
	// Destroy는 포인터일 때만 호출하는 걸로
}

void CObjectManager::Update(const _float& dt, eSCENE tag)
{
	SCENE_MAP::const_iterator iter = m_mapLayer.find(tag);
	if (iter != m_mapLayer.end())
	{
		LAYER_MAP* pMap = iter->second;
		for (int j = 0; j < LAYER_END; ++j)
		{
			LAYER_MAP::const_iterator iter2 = pMap->find((eLAYER)j);
			if (iter2 != pMap->end())
			{
				CLayer* pLayer = iter2->second;
				if (nullptr != pLayer)
					pLayer->Update(dt);
			}
		}
		pMap->clear();
	}
}

void CObjectManager::AddGameObject(eSCENE, eLAYER, CGameObject*)
{
}
