#include "ComponentManager.h"
#include "Component.h"


USING(Engine)
SINGLETON_FUNCTION(ComponentManager)

ComponentManager::ComponentManager()
{
	m_mapMesh.clear();
	m_mapShader.clear();
	m_baseTransform = nullptr;
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::Destroy()
{
	unordered_map<eMESH, CComponent*>::iterator iter;
	for (iter = m_mapMesh.begin(); iter != m_mapMesh.end(); ++iter)
	{
		if (nullptr != iter->second)
			iter->second->Destroy();
	}
	m_mapMesh.clear();

	unordered_map<eSHADER, CComponent*>::iterator iter2;
	for (iter2 = m_mapShader.begin(); iter2 != m_mapShader.end(); ++iter2)
	{
		if (nullptr != iter2->second)
			iter2->second->Destroy();
	}
	m_mapShader.clear();

	unordered_map<eTEXTURE, CComponent*>::iterator iter3;
	for (iter3 = m_mapTexture.begin(); iter3 != m_mapTexture.end(); ++iter3)
	{
		if (nullptr != iter3->second)
			iter3->second->Destroy();
	}
	m_mapShader.clear();

	if (nullptr != m_baseTransform)
		m_baseTransform->Destroy();

	DestroyInstance();
}

RESULT ComponentManager::AddMesh(eMESH tag, Engine::CComponent* pComponent)
{
	if (nullptr == pComponent)
		return PK_COMPONENT_NULLPTR;

	unordered_map<eMESH, CComponent*>::iterator iter = m_mapMesh.find(tag);
	if (iter == m_mapMesh.end())
		m_mapMesh.insert(unordered_map<eMESH, CComponent*>::value_type(tag, pComponent));
	else
		return PK_COMPONENT_EXIST;

	return PK_NOERROR;
}

RESULT ComponentManager::AddShader(eSHADER tag, Engine::CComponent* pComponent)
{
	if (nullptr == pComponent)
		return PK_COMPONENT_NULLPTR;

	unordered_map<eSHADER, CComponent*>::iterator iter = m_mapShader.find(tag);
	if (iter == m_mapShader.end())
		m_mapShader.insert(unordered_map<eSHADER, CComponent*>::value_type(tag, pComponent));
	else
		return PK_COMPONENT_EXIST;

	return PK_NOERROR;
}

RESULT ComponentManager::AddTexture(eTEXTURE tag, Engine::CComponent* pComponent)
{
	if (nullptr == pComponent)
		return PK_COMPONENT_NULLPTR;

	unordered_map<eTEXTURE, CComponent*>::iterator iter = m_mapTexture.find(tag);
	if (iter == m_mapTexture.end())
		m_mapTexture.insert(unordered_map<eTEXTURE, CComponent*>::value_type(tag, pComponent));
	else
		return PK_COMPONENT_EXIST;

	return PK_NOERROR;
}

RESULT ComponentManager::AddTransform(Engine::CComponent* pComponent)
{
	if (nullptr == pComponent)
		return PK_COMPONENT_NULLPTR;

	if (nullptr != m_baseTransform)
		return PK_COMPONENT_EXIST;

	m_baseTransform = pComponent;

	return PK_NOERROR;
}

CComponent* ComponentManager::CloneMesh(eMESH tag)
{
	CComponent* pComponent = nullptr;

	unordered_map<eMESH, CComponent*>::iterator iter = m_mapMesh.find(tag);
	if (iter != m_mapMesh.end())
		pComponent = iter->second->Clone();

	return pComponent;
}

CComponent* ComponentManager::CloneShader(eSHADER tag)
{
	CComponent* pComponent = nullptr;

	unordered_map<eSHADER, CComponent*>::iterator iter = m_mapShader.find(tag);
	if (iter != m_mapShader.end())
		pComponent = iter->second->Clone();

	return pComponent;
}

Engine::CComponent* ComponentManager::CloneTexture(eTEXTURE tag)
{
	CComponent* pComponent = nullptr;

	unordered_map<eTEXTURE, CComponent*>::iterator iter = m_mapTexture.find(tag);
	if (iter != m_mapTexture.end())
		pComponent = iter->second->Clone();

	return pComponent;
}

CComponent* ComponentManager::CloneTransform()
{
	CComponent* pComponent = nullptr;

	if (nullptr != m_baseTransform)
		pComponent = m_baseTransform->Clone();

	return pComponent;
}
