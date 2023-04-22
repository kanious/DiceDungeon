#include "pch.h"
#include "..\Headers\Tile.h"
#include "..\Headers\GameObject.h"

USING(std)
USING(glm)
USING(Engine)

CTile::CTile()
	: m_iIdx(0), m_vPos(vec3(0.f)), m_bIsBlocked(false), m_pObj(nullptr)
{
	memset(m_neighborNodes, 0, sizeof(m_neighborNodes));
}

CTile::~CTile()
{
}

void CTile::SetEnableTile(_bool value)
{
	if (nullptr != m_pObj)
		m_pObj->SetEnable(value);
}

// Call instead of destructor to manage class internal data
void CTile::Destroy()
{
}

// Initialize
RESULT CTile::Ready(_uint idx, vec3 vPos, _bool isBlocked, CGameObject* pObj)
{
	m_iIdx = idx;
	m_vPos = vPos;
	m_bIsBlocked = isBlocked;
	m_pObj = pObj;

	return PK_NOERROR;
}

// Create an instance
CTile* CTile::Create(_uint idx, vec3 vPos, _bool isBlocked, CGameObject* pObj)
{
	CTile* pInstance = new CTile();
	if (PK_NOERROR != pInstance->Ready(idx, vPos, isBlocked, pObj))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
