#ifndef _TILE_H_
#define _TILE_H_

#include "Base.h"
#include "glm\vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CGameObject;
class ENGINE_API CTile : public CBase
{
public:
	enum TILE_DIR
	{
		ZP,
		ZP_XP,
		XP,
		ZN_XP,
		ZN,
		ZN_XN,
		XN,
		ZP_XN,
		TILE_DIR_END
	};

public:
	_uint			m_iIdx;
	glm::vec3		m_vPos;
	CTile*			m_neighborNodes[8];
	CGameObject*	m_pObj;

	_bool			m_bIsBlocked;

private:
	explicit CTile();
	virtual ~CTile();

public:
	_bool GetBlocked()					{ return m_bIsBlocked; }
	CGameObject* GetTileObject()		{ return m_pObj; }
	void SetEnableTile(_bool value);

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint idx, glm::vec3 vPos, _bool isBlocked, CGameObject* pObj);
public:
	// Create an instance
	static CTile*Create(_uint idx, glm::vec3 vPos, _bool isBlocked, CGameObject* pObj);
};

NAMESPACE_END

#endif //_TILE_H_