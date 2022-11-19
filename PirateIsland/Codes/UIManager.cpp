#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "Scene.h"
#include <sstream>
#include <atlconv.h>


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

void UIManager::Update(const _float& dt)
{
}

void UIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	//

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT UIManager::Ready(CScene* pScene)
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

_bool UIManager::GetCursorIsOnTheUI()
{
	return GetIO().WantCaptureMouse;
}
