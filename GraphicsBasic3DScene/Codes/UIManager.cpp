#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "MapEditorUIManager.h"
#include "SoundUIManager.h"
#include "Scene.h"


SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)

UIManager::UIManager()
	: m_pMapEditorUI(nullptr), m_pSoundUI(nullptr), m_pCurrentScene(nullptr)
{

}

UIManager::~UIManager()
{

}

void UIManager::Destroy()
{
	SafeDestroy(m_pMapEditorUI);
	SafeDestroy(m_pSoundUI);
}

void UIManager::RenderUI()
{

}

RESULT UIManager::Ready(CScene* pScene)
{
	m_pCurrentScene = pScene;
	//m_pMapEditorUI = new MapEditorUIManager();
}