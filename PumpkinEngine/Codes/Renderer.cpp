#include "pch.h"
#include "..\Headers\Renderer.h"
#include "..\Headers\GameObject.h"


USING(Engine)
USING(std)
SINGLETON_FUNCTION(CRenderer)

CRenderer::CRenderer()
{
}

CRenderer::~CRenderer()
{
}

void CRenderer::Destroy()
{
	ClearAllRenderObjList();
}


void CRenderer::Render()
{
	vector<CGameObject*>::iterator iter;
	for (iter = m_vecRenderObj.begin(); iter != m_vecRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
	ClearAllRenderObjList();
}
void CRenderer::AddRenderObj(CGameObject* pInstance)
{
	if (nullptr == pInstance)
		return;

	m_vecRenderObj.push_back(pInstance);
}

void CRenderer::ClearAllRenderObjList()
{
	m_vecRenderObj.clear();
}
