#pragma once

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

public:
	void AddRenderObj(CGameObject*);
	void RemoveRenderObj();
};

NAMESPACE_END
