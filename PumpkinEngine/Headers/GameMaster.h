#ifndef _GAMEMASTER_H_
#define _GAMEMASTER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CScene;
class CRenderer;

class ENGINE_API CGameMaster : public CBase
{
	SINGLETON(CGameMaster)

private:
	CScene*				m_pCurrentScene;
	CRenderer*			m_pRenderer;


private:
	explicit CGameMaster();
	virtual ~CGameMaster();
public:
	void Update(const _float& dt);
	void Render();
private:
	void Destroy();

public:
	CScene* GetCurrentScene()		{ return m_pCurrentScene; }
	void SetCurrentScene(CScene* pScene);
};

NAMESPACE_END

#endif //_GAMEMASTER_H_