#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "Define.h"
#include "DiceMaster.h"
#include "iPhysicsWorld.h"

#include <sstream>
#include <iomanip>

SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

// Call instead of destructor to manage class internal data
void UIManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

// Basic Render Function
void UIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	//ImVec2 screen = ImVec2((_float)COpenGLDevice::GetInstance()->GetWidthSize(), (_float)COpenGLDevice::GetInstance()->GetHeightSize());
	ImVec2 screen = ImVec2(200.f, 100.f);
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(screen);
	if (Begin("UI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar))
	{
		Text("UI");
		Separator();

		if (Button("Roll2", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(2);
		}
		SameLine(65.f);
		if (Button("Roll3", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(3);
		}
		SameLine(120.f);
		if (Button("Roll5", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(5);
		}

		Text("Rolled Dice:"); SameLine(115.f); 
		stringstream ss;
		ss << CDiceMaster::GetInstance()->GetRolledDiceCount();
		Text(ss.str().c_str());

		Text("AP:"); SameLine(115.f);
		ss.str("");
		ss << CDiceMaster::GetInstance()->GetAP();
		Text(ss.str().c_str());
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

// Initialize
RESULT UIManager::Ready(iPhysicsWorld* pWorld)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

	m_pPWorld = pWorld;

	return PK_NOERROR;
}