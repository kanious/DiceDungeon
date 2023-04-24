#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;
class CDeferredFBO;
// Only game objects registered here are rendered
class ENGINE_API CRenderer : public CBase
{
	SINGLETON(CRenderer)

private:
	std::vector<CGameObject*>		m_vecRenderObj;
	std::vector<CGameObject*>		m_vecTRenderObj;

	CDeferredFBO*					m_pDeferred;

	_bool							m_bShowDebug;
	_bool							m_bRenderDeferred;


private:
	explicit CRenderer();
	virtual ~CRenderer();
	void Destroy();
public:
	// Basic Render Function, translucent objects are rendered later than other objects
	void Render();
	void RenderForward();
	void RenderDeferred();
	void SetShowDebug(_bool value)		{ m_bShowDebug = value; }
	void ToggleShowDebug()				{ m_bShowDebug = !m_bShowDebug; }
	void SetRenderDeferred(_bool value) { m_bRenderDeferred = value; }
	void SetNoiseOption(_bool on);
	void SetNoiseAmount(float amount);
	_bool GetNoiseOption();
	_float GetNoiseAmount();

public:
	// Register objects that need to be rendered
	void AddRenderObj(CGameObject* pInstance, _bool isTransparent = false);
private:
	// Empty container
	void ClearAllRenderObjList();
};

NAMESPACE_END

#endif //_RENDERER_H_