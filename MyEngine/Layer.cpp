#include "pch.h"
#include "Layer.h"


USING(Engine)

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
}

void CLayer::Awake()
{
	m_vecGameObjects.clear();
}

void CLayer::Update(const _float& dt)
{
	vector<CGameObject*>::iterator iter = m_vecGameObjects.begin();
	for (iter; iter != m_vecGameObjects.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Update(dt);
	}
}

void CLayer::Render()
{
}

void CLayer::Enable()
{
}

void CLayer::Disable()
{
}

void CLayer::Destroy()
{
	vector<CGameObject*>::iterator iter = m_vecGameObjects.begin();
	for (iter; iter != m_vecGameObjects.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Destroy();
	}
	m_vecGameObjects.clear();

	delete this;
}

void CLayer::SetTag(eLAYER tag)
{
	m_tag = tag;
}

eLAYER CLayer::GetTag()
{
	return m_tag;
}

void CLayer::AddGameObject(CGameObject* obj)
{
	if (nullptr == obj)
		return;

	m_vecGameObjects.push_back(obj);
}

CLayer* CLayer::Create(eLAYER tag)
{
	CLayer* pInstance = new CLayer;
	if (nullptr != pInstance)
		pInstance->SetTag(tag);

	return pInstance;
}
