#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;

// Only game objects registered here are rendered
class ENGINE_API CRenderer : public CBase
{
	SINGLETON(CRenderer)

private:
	std::vector<CGameObject*>		m_vecRenderObj;
	std::vector<CGameObject*>		m_vecTRenderObj;


private:
	explicit CRenderer();
	virtual ~CRenderer();
	void Destroy();
public:
	void Render();

public:
	void AddRenderObj(CGameObject* pInstance, _bool isTransparent = false);
private:
	void ClearAllRenderObjList();
};

NAMESPACE_END

#endif //_RENDERER_H_