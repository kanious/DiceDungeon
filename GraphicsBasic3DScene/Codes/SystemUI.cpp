#include "SystemUI.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "UIManager.h"
#include "OpenGLDevice.h"
#include "GameMaster.h"
#include "PumpkinString.h"
#include <sstream>


USING(Engine)
USING(ImGui)
USING(std)

SystemUI::SystemUI()
{
	m_pUIManager = UIManager::GetInstance();
	m_pString = PumpkinString::GetInstance(); m_pString->AddRefCnt();
}

SystemUI::~SystemUI()
{
}

void SystemUI::Destroy()
{
	SafeDestroy(m_pString);
}

void SystemUI::RenderUI()
{
	if (nullptr == m_pUIManager || nullptr == m_pString)
		return;

	_int iWidth = (_float)COpenGLDevice::GetInstance()->GetWidthSize();
	_int iHeight = (_float)COpenGLDevice::GetInstance()->GetHeightSize();

	SetNextWindowPos(ImVec2((iWidth >> 1) - 100.f, (iHeight >> 1) - 200.f));
	SetNextWindowSize(ImVec2(200.f, 400.f));

	stringstream ss;
	ss << m_pString->GetString("TEXT_01") << "##systemWindow";
	if (Begin(ss.str().c_str(), (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		ss.str("");
		ss << m_pString->GetString("TEXT_02") << "##Back To Game";
		if (Button(ss.str().c_str(), ImVec2(185.f, 0.f)))
		{
			if (nullptr != m_pUIManager)
				m_pUIManager->SetAllUIClose();
		}

		Text(" ");

		ss.str("");
		if (m_pUIManager->GetUIOpened(UIManager::UI_MAP_EDITOR_WINDOW))
			ss << m_pString->GetString("TEXT_03") << "##Close Map Editor";
		else
			ss << m_pString->GetString("TEXT_04") << "##Open Map Editor";
		if (Button(ss.str().c_str(), ImVec2(185.f, 0.f)))
		{
			m_pUIManager->SetUI(UIManager::UI_MAP_EDITOR_WINDOW);
		}

		ss.str("");
		if (m_pUIManager->GetUIOpened(UIManager::UI_SOUND_EDITOR_WINDOW))
			ss << m_pString->GetString("TEXT_05") << "##Close Sound Editor";
		else
			ss << m_pString->GetString("TEXT_06") << "##Open Sound Editor";
		if (Button(ss.str().c_str(), ImVec2(185.f, 0.f)))
		{
			m_pUIManager->SetUI(UIManager::UI_SOUND_EDITOR_WINDOW);
		}

		Text(" ");
		Text(m_pString->GetString("TEXT_07").c_str());
		if (Button("English", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_en);
		}
		if (Button(u8"español", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_sp);
		}
		if (Button(u8"한국어", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_ko);
		}
		if (Button(u8"日本語", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_jp);
		}
		if (Button(u8"Русский", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_ru);
		}
		if (Button(u8"Ελληνικά", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_gr);
		}
		if (Button(u8"ไทย", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_th);
		}
		if (Button(u8"Tiếng Việt", ImVec2(185.f, 0.f)))
		{
			m_pString->LanguageChange(PumpkinString::lang_vt);
		}

		Text(" ");
		Text(" ");
		ss.str("");
		ss << m_pString->GetString("TEXT_08") << "##Quit";
		if (Button(ss.str().c_str(), ImVec2(185.f, 0.f)))
		{
			CGameMaster::GetInstance()->SetGameClose(true);
		}
	}
	End();
}

RESULT SystemUI::Ready()
{
	return PK_NOERROR;
}

SystemUI* SystemUI::Create()
{
	SystemUI* pInstance = new SystemUI();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
