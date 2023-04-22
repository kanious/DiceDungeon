#include "pch.h"
#include "..\Headers\GameMaster.h"
#include "..\Headers\ComponentMaster.h"
#include "..\Headers\InputDevice.h"
#include "..\Headers\DiceMaster.h"
#include "..\Headers\OpenGLDevice.h"
#include "..\Headers\Renderer.h"
#include "..\Headers\Timer.h"
#include "..\Headers\XMLParser.h"
#include "..\Headers\JsonParser.h"
#include "..\Headers\SoundMaster.h"
#include "..\Headers\CollisionMaster.h"
#include "..\Headers\LightMaster.h"
#include "..\Headers\AnimationData.h"
#include "..\Headers\LuaBrain.h"
#include "..\Headers\SQLiteDBHelper.h"
#include "..\Headers\TileMaster.h"

#include "..\Headers\Scene.h"

USING(Engine)
SINGLETON_FUNCTION(CGameMaster)

CGameMaster::CGameMaster()
	: m_bGameClose(false)
{
	m_pCurrentScene = nullptr;
}

CGameMaster::~CGameMaster()
{
}

// Basic Update Function, update current scene
void CGameMaster::Update(const _float& dt)
{
	if (nullptr == m_pCurrentScene)
		return;

	m_pCurrentScene->Update(dt);
}

// Basic Render Function, render renderer (and scene if needed)
void CGameMaster::Render()
{
	if (nullptr != m_pRenderer)
		m_pRenderer->Render();

	if (nullptr != m_pCurrentScene)
		m_pCurrentScene->Render();
}

// Call instead of destructor to manage class internal data
void CGameMaster::Destroy()
{
	SafeDestroy(m_pCurrentScene);

	SafeDestroy(CXMLParser::GetInstance());
	SafeDestroy(CJsonParser::GetInstance());
	SafeDestroy(CComponentMaster::GetInstance());
	SafeDestroy(CInputDevice::GetInstance());
	SafeDestroy(CDiceMaster::GetInstance());
	SafeDestroy(COpenGLDevice::GetInstance());
	SafeDestroy(CRenderer::GetInstance());
	SafeDestroy(CTimer::GetInstance());
	SafeDestroy(CSoundMaster::GetInstance());
	SafeDestroy(CCollisionMaster::GetInstance());
	SafeDestroy(CLightMaster::GetInstance());
	SafeDestroy(CAnimationData::GetInstance());
	SafeDestroy(CLuaBrain::GetInstance());
	SafeDestroy(CSQLiteDBHelper::GetInstance());
	SafeDestroy(CTileMaster::GetInstance());

	m_pRenderer = nullptr;
}

// Initialize
void CGameMaster::Ready()
{
	m_pRenderer = CRenderer::GetInstance();
}

void CGameMaster::SetRenderType(_bool deferred)
{
	if (nullptr != m_pRenderer)
		m_pRenderer->SetRenderDeferred(deferred);
}

// Set current scene information
void CGameMaster::SetCurrentScene(CScene* pScene)
{
	SafeDestroy(m_pCurrentScene);
	
	m_pCurrentScene = pScene;
}
