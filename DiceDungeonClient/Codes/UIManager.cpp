#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "Define.h"

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

void UIManager::Destroy()
{
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
	//ImVec2 screen = ImVec2(350.f, 275.f);
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(screen);
	if (Begin("UI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar))
	{
		PushStyleColor(ImGuiCol_Text, IM_COL32(0, 0, 0, 255));
		Text("Mouse Click (LB) : Select Target (if click background, target will be released)");
		Text(" ");
		Text("Right Arrow : Next Animation Frame");
		Text("Left Arrow : Previous Animation Frame");
		Text("R : Reverse Animation");
		Text("Space : Animation Play/Pause Toggle");
		Text("1 ~ 5 : Set Animation Speed (1x ~ 5x, default is x3)");
		Text(" ");
		Text("ESC : Close the program");
		Text(" ");
		Text(" ");
		Text(" ");
		Text("< Color Information >");
		Text("No Easing : White");
		Text("Ease In : Red");
		Text("Ease Out : Yellow");
		Text("Ease InOut : Blue");
		Text("Selected : Dark Grey");
		PopStyleColor();
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