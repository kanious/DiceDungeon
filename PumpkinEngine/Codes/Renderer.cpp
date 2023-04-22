#include "pch.h"
#include "..\Headers\Renderer.h"
#include "..\Headers\GameObject.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\OpenGLDevice.h"
#include "..\Headers\DeferredFBO.h"


USING(Engine)
USING(std)
SINGLETON_FUNCTION(CRenderer)

CRenderer::CRenderer()
	: m_bShowDebug(false), m_bRenderDeferred(true)
{
	COpenGLDevice::GetInstance()->GetWindowSize();
	_int width = COpenGLDevice::GetInstance()->GetWidthSize();
	_int height = COpenGLDevice::GetInstance()->GetHeightSize();

	printf("Renderer making start\n");
	m_pDeferred = CDeferredFBO::Create("Deferred", width, height);
}

CRenderer::~CRenderer()
{
}

void CRenderer::Destroy()
{
	SafeDestroy(m_pDeferred);

	ClearAllRenderObjList();
}

// Basic Render Function, translucent objects are rendered later than other objects
void CRenderer::Render()
{
	if (nullptr == m_pDeferred)
		return;

	if (m_bRenderDeferred)
		RenderDeferred();
	else
		RenderForward();

	ClearAllRenderObjList();
}

void CRenderer::RenderForward()
{
	vector<CGameObject*>::iterator iter;
	for (iter = m_vecRenderObj.begin(); iter != m_vecRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
	for (iter = m_vecTRenderObj.begin(); iter != m_vecTRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
}

void CRenderer::RenderDeferred()
{
	m_pDeferred->RenderStart();

	vector<CGameObject*>::iterator iter;
	for (iter = m_vecRenderObj.begin(); iter != m_vecRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
	m_pDeferred->RenderEnd();

	m_pDeferred->Render();
	if (m_bShowDebug)
		m_pDeferred->Render_Debug();

	for (iter = m_vecTRenderObj.begin(); iter != m_vecTRenderObj.end(); ++iter)
	{
		if (nullptr != *iter)
			(*iter)->Render();
	}
}

// Register objects that need to be rendered
void CRenderer::AddRenderObj(CGameObject* pInstance, _bool isTransparent)
{
	if (nullptr == pInstance)
		return;

	if (!isTransparent)
		m_vecRenderObj.push_back(pInstance);
	else
		m_vecTRenderObj.push_back(pInstance);
}

// Empty container
void CRenderer::ClearAllRenderObjList()
{
	m_vecRenderObj.clear();
	m_vecTRenderObj.clear();
}
