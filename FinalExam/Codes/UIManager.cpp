#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "PhysicsSystem.h"
#include "Target.h"
#include "Define.h"

#include <sstream>
#include <iomanip>

SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)

UIManager::UIManager()
{
	m_pPhysicsSystem = PhysicsSystem::GetInstance(); m_pPhysicsSystem->AddRefCnt();
}

UIManager::~UIManager()
{
}

void UIManager::Destroy()
{
	SafeDestroy(m_pPhysicsSystem);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

void UIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	ImVec2 screen = ImVec2((_float)COpenGLDevice::GetInstance()->GetWidthSize(), (_float)COpenGLDevice::GetInstance()->GetHeightSize());
	SetMouseCursor(ImGuiMouseCursor_None);
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(screen);
	if (Begin("Game Info", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar))
	{
		stringstream ss;

		if (SHOT_MODE == m_pPhysicsSystem->GetGameMode())
			Text("Game Mode: Shot");
		else
		{
			ss << "Game Mode: Hover [" << HOVER_TIME << "s]";
			Text(ss.str().c_str());
		}

		ss.str("");
		ss << "Killed Targets: " << m_pPhysicsSystem->GetKillCount();
		Text(ss.str().c_str());

		screen.x /= 2;
		screen.y /= 2;

		ImDrawList* draw = ImGui::GetBackgroundDrawList();

		Target* hoverTarget = m_pPhysicsSystem->GetHoverTarget();
		if (nullptr != hoverTarget)
		{
			ImVec2 p1 = screen;
			p1.x -= 10.f;
			p1.y -= 35.f;
			SetNextWindowPos(p1);

			ss.str("");
			ss.setf(std::ios::fixed);
			ss.precision(2);
			ss << HOVER_TIME - hoverTarget->GetHoverElaspedTime() << "s";
			draw->AddText(p1, IM_COL32(0, 255, 0, 255), ss.str().c_str());
		}


		
		screen.x -= 10.f;
		ImVec2 p2 = screen;
		p2.x += 20.f;
		draw->AddLine(screen, p2, IM_COL32(0, 255, 0, 255), 2.f);

		screen.x += 10.f;
		screen.y -= 10.f;
		p2.x -= 10.f;
		p2.y += 10.f;
		draw->AddLine(screen, p2, IM_COL32(0, 255, 0, 255), 2.f);
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT UIManager::Ready()
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

	return PK_NOERROR;
}