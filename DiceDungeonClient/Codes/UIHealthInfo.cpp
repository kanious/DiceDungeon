#include "UIHealthInfo.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "OpenGLDefines.h"
#include "GameObject.h"

#include <sstream>
#include <iomanip>

USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

_int UIHealthInfo::g_Num = 0;
UIHealthInfo::UIHealthInfo()
	: m_pObj(nullptr), m_name("")
{
}

UIHealthInfo::~UIHealthInfo()
{
}

// Call instead of destructor to manage class internal data
void UIHealthInfo::Destroy()
{
}

// Basic Render Function
void UIHealthInfo::RenderUI()
{
	if (nullptr == m_pObj)
		return;

	_float width = (_float)COpenGLDevice::GetInstance()->GetWidthSize();
	_float height = (_float)COpenGLDevice::GetInstance()->GetHeightSize();
	vec3 vPos = m_pObj->GetPosition();
	vPos.y += 7.f;

	mat4x4 matWorld, matView, matProj;
	matWorld = mat4x4(1.f);
	matWorld = translate(matWorld, vPos);
	matView = COpenGLDevice::GetInstance()->GetViewMatrix();
	matProj = COpenGLDevice::GetInstance()->GetProjMatrix();
	vec4 screenPos = vec4(0.f, 0.f, 0.f, 1.f);
	screenPos = matProj * matView * matWorld * screenPos;
	_float screenX = screenPos.x / screenPos.w;
	_float screenY = screenPos.y / screenPos.w;
	screenX = (screenX + 1) * 0.5f * width;
	screenY = (screenY + 1) * 0.5f;
	screenY = 1 - screenY;
	screenY = (screenY * height);// -100.f;

	ImVec2 windowSize = ImVec2(100.f, 50.f);
	SetNextWindowPos(ImVec2(screenX - (windowSize.x / 2.f), screenY - (windowSize.y / 2.f)));
	SetNextWindowSize(windowSize);
	if (Begin(m_windowName.c_str(), (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar))
	{
		Text(m_name.c_str());

		stringstream ss;
		ss << "HP  " << m_pObj->GetCurHP() << "/" << m_pObj->GetMaxHP();
		Text(ss.str().c_str());

		End();
	}
}

// Initialize
RESULT UIHealthInfo::Ready(CGameObject* pObj)
{
	m_pObj = pObj;
	if (nullptr != m_pObj)
	{
		m_name = m_pObj->GetMeshName();

		stringstream ss;
		ss << m_name << g_Num;
		m_windowName = ss.str();
		++g_Num;
	}

	return PK_NOERROR;
}

UIHealthInfo* UIHealthInfo::Create(CGameObject* pObj)
{
	UIHealthInfo* pInstance = new UIHealthInfo();
	if (PK_NOERROR != pInstance->Ready(pObj))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
