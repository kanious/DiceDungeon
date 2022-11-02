#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "SystemUI.h"
#include "MapEditorUI.h"
#include "SoundUI.h"
#include "Scene.h"


SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)

UIManager::UIManager()
	: m_pSystemUI(nullptr), m_pMapEditorUI(nullptr), m_pSoundUI(nullptr)
{

}

UIManager::~UIManager()
{

}

void UIManager::Destroy()
{
	SafeDestroy(m_pSystemUI);
	SafeDestroy(m_pMapEditorUI);
	SafeDestroy(m_pSoundUI);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

void UIManager::Update(const _float& dt)
{
	if (m_bUIOpen[UI_MAP_EDITOR_WINDOW])
	{
		if (nullptr != m_pMapEditorUI)
			m_pMapEditorUI->Update(dt);
	}
}

void UIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	if (m_bUIOpen[UI_SYSTEM_MENU_WINDOW])
	{
		if (nullptr != m_pSystemUI)
			m_pSystemUI->RenderUI();
	}

	if (m_bUIOpen[UI_MAP_EDITOR_WINDOW])
	{
		if (nullptr != m_pMapEditorUI)
			m_pMapEditorUI->RenderUI();
	}
	else if (m_bUIOpen[UI_SOUND_EDITOR_WINDOW])
	{
		if (nullptr != m_pSoundUI)
			m_pSoundUI->RenderUI();
	}

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT UIManager::Ready(CScene* pScene)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	ImFont* font = io.Fonts->AddFontDefault();
	ImFontConfig config;
	config.MergeMode = true;

	static const ImWchar greek_ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0370, 0x03FF, // Greek and Coptic
		0,
	};
	
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Korean\\PoorStory-Regular.ttf", 16.f, &config, io.Fonts->GetGlyphRangesKorean());
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Japanese\\MPLUS1p-Regular.ttf", 18.f, &config, io.Fonts->GetGlyphRangesJapanese());
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Noto_Sans\\NotoSans-Regular.ttf", 18.f, &config, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Thai\\Sarabun-Regular.ttf", 18.f, &config, io.Fonts->GetGlyphRangesThai());
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Greek\\OpenSans-VariableFont_wdth,wght.ttf", 18.f, &config, greek_ranges);
	io.Fonts->AddFontFromFileTTF("Assets\\Fonts\\Noto_Sans\\NotoSans-Regular.ttf", 18.f, &config, io.Fonts->GetGlyphRangesVietnamese());
	io.Fonts->Build();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

	m_pSystemUI = SystemUI::Create();
	m_pMapEditorUI = MapEditorUI::Create(pScene);
	m_pSoundUI = SoundUI::Create();

	return PK_NOERROR;
}

_bool UIManager::GetCursorIsOnTheUI()
{
	return GetIO().WantCaptureMouse;
}

_bool UIManager::GetAnyUIOpened()
{
	_bool uiOpened = false;

	for (int i = 0; i < UI_END; ++i)
	{
		if (m_bUIOpen[i])
			return true;
	}

	return uiOpened;
}

void UIManager::SetUI(eUIType type)
{
	_bool previous = m_bUIOpen[type];
	m_bUIOpen[type] = !previous;

	if (UI_SYSTEM_MENU_WINDOW != type)
	{
		if (!previous)
		{
			if (UI_MAP_EDITOR_WINDOW == type)
				m_bUIOpen[UI_SOUND_EDITOR_WINDOW] = false;
			else if (UI_SOUND_EDITOR_WINDOW == type)
				m_bUIOpen[UI_MAP_EDITOR_WINDOW] = false;
		}
		m_bUIOpen[UI_SYSTEM_MENU_WINDOW] = false;
	}
}

void UIManager::SetAllUIClose()
{
	for (int i = 0; i < UI_END; ++i)
	{
		m_bUIOpen[i] = false;
	}
}
