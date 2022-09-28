#ifndef _RENDERMANAGER_H_
#define _RENDERMANAGER_H_

#include "Defines.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;

class ENGINE_API CRenderManager
{
	SINGLETON(CRenderManager)

private:
	vector<CGameObject*>		m_vecRenderObj;


private:
	explicit CRenderManager();
	~CRenderManager();

private:
	void Awake();
public:
	void Render();
	void Destroy();
	void DestroyManager();

public:
	void AddRenderObj(CGameObject*);
	void ClearAllRenderObjList();
};

NAMESPACE_END

#endif //_RENDERMANAGER_H_