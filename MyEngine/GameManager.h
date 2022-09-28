#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_

#include "Defines.h"
#include "Scene.h"
#include "RenderManager.h"

NAMESPACE_BEGIN(Engine)

class CScene;
class CRenderManager;

class ENGINE_API CGameManager
{
	SINGLETON(CGameManager)

private:
	CScene*				m_pCurrentScene;
	CRenderManager*		m_pRenderManager;


private:
	explicit CGameManager();
	~CGameManager();

private:
	void Awake();
public:
	void Update(const _float&);
	void Render();
	void Destroy();
	void DestroyManager();

public:
	void SetCurrentScene(CScene*);
};

NAMESPACE_END

#endif //_GAMEMANAGER_H_