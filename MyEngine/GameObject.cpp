#include "pch.h"
#include "GameObject.h"
#include "Scene.h"


USING(Engine)

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
	m_UUID = Utils::UUIDTest();
	memset(m_CompArr, 0, sizeof(m_CompArr));
}

void CGameObject::Update(const _float& dt)
{
	for (int i = 0; i < COMP_END; ++i)
	{
		if (nullptr != m_CompArr[i])
		{
			if (m_CompArr[i]->GetEnable())
				m_CompArr[i]->Update(dt);
		}
	}
}

void CGameObject::Render()
{
	for (int i = 0; i < COMP_END; ++i)
	{
		if (nullptr != m_CompArr[i])
		{
			if (m_CompArr[i]->GetEnable())
				m_CompArr[i]->Render();
		}
	}
}

void CGameObject::Destroy()
{
	::RpcStringFreeW((RPC_WSTR*)&m_UUID);
	m_UUID = NULL;
}

void CGameObject::Enable()
{
	m_bEnable = true;
}

void CGameObject::Disable()
{
	m_bEnable = false;
}

void CGameObject::SetObjectType(eOBJTYPE type)
{
	m_eType = type;
}

void CGameObject::AttachComponent(eCOMPONENT tag, CComponent* pInstance)
{
	if (nullptr != pInstance)
		m_CompArr[tag] = pInstance;
}

void CGameObject::DetachComponent(eCOMPONENT tag)
{
	if (nullptr != m_CompArr[tag])
	{
		m_CompArr[tag]->Destroy();
		m_CompArr[tag] = nullptr;
	}
}

CComponent* CGameObject::GetComponent(eCOMPONENT tag)
{
	return m_CompArr[tag];
}
