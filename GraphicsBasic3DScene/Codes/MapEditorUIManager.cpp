#include "MapEditorUIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "Enums.h"
#include "OpenGLDevice.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Scene.h"
#include "GameObject.h"
#include "BGObject.h"
#include "glm\vec3.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>


SINGLETON_FUNCTION(MapEditorUIManager)
USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

MapEditorUIManager::MapEditorUIManager()
	: m_pScene(nullptr), m_ppTargetObject(nullptr)
{
	m_vecMeshList.clear();
	m_vecInputList.clear();

	ZeroMemory(m_chPosX, sizeof(m_chPosX));
	ZeroMemory(m_chPosY, sizeof(m_chPosY));
	ZeroMemory(m_chPosZ, sizeof(m_chPosZ));
	ZeroMemory(m_chRotX, sizeof(m_chRotX));
	ZeroMemory(m_chRotY, sizeof(m_chRotY));
	ZeroMemory(m_chRotZ, sizeof(m_chRotZ));
	ZeroMemory(m_chScaleX, sizeof(m_chScaleX));
	ZeroMemory(m_chScaleY, sizeof(m_chScaleY));
	ZeroMemory(m_chScaleZ, sizeof(m_chScaleZ));
}

_bool MapEditorUIManager::GetCursorIsOnTheUI()
{
	return GetIO().WantCaptureMouse;
}

MapEditorUIManager::~MapEditorUIManager()
{
}

void MapEditorUIManager::Destroy()
{
	m_vecMeshList.clear();

	vector<char*>::iterator iter;
	for (iter = m_vecInputList.begin(); iter != m_vecInputList.end(); ++iter)
		delete *iter;
	m_vecInputList.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();

	DestroyInstance();
}

void MapEditorUIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();
	
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(ImVec2(400.f, (_float)COpenGLDevice::GetInstance()->GetHeightSize()));
	if (Begin("Pumpkins Map Editor", (bool*)0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		ImVec2 screenSize = GetWindowSize();

		RenderTargetDetailUI(screenSize.x, screenSize.y);
	}
	End();

	SetNextWindowPos(ImVec2(COpenGLDevice::GetInstance()->GetWidthSize() - 360.f, 0.f));
	SetNextWindowSize(ImVec2(360.f, (_float)COpenGLDevice::GetInstance()->GetHeightSize()));
	if (Begin("Mesh List", (bool*)0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		ImVec2 screenSize = GetWindowSize();

		RenderMeshList(screenSize.x, screenSize.y);
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT MapEditorUIManager::Ready(CScene* pScene, BGObject** ppTarget)
{
	m_pScene = pScene;
	m_ppTargetObject = ppTarget;

	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();
	
	unordered_map<string, string>* pMeshMap = CComponentMaster::GetInstance()->GetMeshMap();
	unordered_map<string, string>::iterator iter;
	for (iter = pMeshMap->begin(); iter != pMeshMap->end(); ++iter)
	{
		m_vecMeshList.push_back(iter->first);
		char* inputText = new char[128];
		memset(inputText, 0, sizeof(*inputText));
		strcpy_s(inputText, iter->second.length() + 1, iter->second.c_str());
		m_vecInputList.push_back(inputText);
	}

	return PK_NOERROR;
}

void MapEditorUIManager::RenderTargetDetailUI(_float screenX, _float screenY)
{
	//if (BeginChild("##TargetSetting", ImVec2(screenX * 0.983f, 200.f)))
	//{
	Text("* Target Detail Setting");
	if (nullptr == *m_ppTargetObject)
	{
		Text("There is no target.");
		//EndChild();
		return;
	}

	Text("Mesh ID:"); SameLine(80.f); Text((*m_ppTargetObject)->GetMeshID().c_str());
	Text(" ");

	vec3 vPos = (*m_ppTargetObject)->GetPosition();
	ConvertFloatToCharArray(m_chPosX, vPos.x);
	ConvertFloatToCharArray(m_chPosY, vPos.y);
	ConvertFloatToCharArray(m_chPosZ, vPos.z);
	vec3 vRot = (*m_ppTargetObject)->GetRotation();
	ConvertFloatToCharArray(m_chRotX, vRot.x);
	ConvertFloatToCharArray(m_chRotY, vRot.y);
	ConvertFloatToCharArray(m_chRotZ, vRot.z);
	vec3 vScale = (*m_ppTargetObject)->GetScale();
	ConvertFloatToCharArray(m_chScaleX, vScale.x);
	ConvertFloatToCharArray(m_chScaleY, vScale.y);
	ConvertFloatToCharArray(m_chScaleZ, vScale.z);

	Text("Position");
	SameLine(80.f); Text("X:"); SameLine(100.f);
	SetNextItemWidth(70); InputText("##PosX", m_chPosX, sizeof(m_chPosX));
	SameLine(180.f); Text("Y:"); SameLine(200.f);
	SetNextItemWidth(70); InputText("##PosY", m_chPosY, sizeof(m_chPosY));
	SameLine(280.f); Text("Z:"); SameLine(300.f);
	SetNextItemWidth(70); InputText("##PosZ", m_chPosZ, sizeof(m_chPosZ));

	Text("Rotation");
	SameLine(80.f); Text("X:"); SameLine(100.f);
	SetNextItemWidth(70); InputText("##RotX", m_chRotX, sizeof(m_chRotX));
	SameLine(180.f); Text("Y:"); SameLine(200.f);
	SetNextItemWidth(70); InputText("##RotY", m_chRotY, sizeof(m_chRotY));
	SameLine(280.f); Text("Z:"); SameLine(300.f);
	SetNextItemWidth(70); InputText("##RotZ", m_chRotZ, sizeof(m_chRotZ));

	Text("Scale");
	SameLine(80.f); Text("X:"); SameLine(100.f);
	SetNextItemWidth(70); InputText("##ScaleX", m_chScaleX, sizeof(m_chScaleX));
	SameLine(180.f); Text("Y:"); SameLine(200.f);
	SetNextItemWidth(70); InputText("##ScaleY", m_chScaleY, sizeof(m_chScaleY));
	SameLine(280.f); Text("Z:"); SameLine(300.f);
	SetNextItemWidth(70); InputText("##ScaleZ", m_chScaleZ, sizeof(m_chScaleZ));

	_float fPosX = vPos.x; _float fPosY = vPos.y; _float fPosZ = vPos.z;
	_float fRotX = vRot.x; _float fRotY = vRot.y; _float fRotZ = vRot.z;
	_float fScaleX = vScale.x; _float fScaleY = vScale.y; _float fScaleZ = vScale.z;
	if (m_chPosX[0] != '\0') fPosX = (_float)atof(m_chPosX);
	if (m_chPosY[0] != '\0') fPosY = (_float)atof(m_chPosY);
	if (m_chPosZ[0] != '\0') fPosZ = (_float)atof(m_chPosZ);
	if (m_chRotX[0] != '\0') fRotX = (_float)atof(m_chRotX);
	if (m_chRotY[0] != '\0') fRotY = (_float)atof(m_chRotY);
	if (m_chRotZ[0] != '\0') fRotZ = (_float)atof(m_chRotZ);
	if (m_chScaleX[0] != '\0') fScaleX = (_float)atof(m_chScaleX);
	if (m_chScaleY[0] != '\0') fScaleY = (_float)atof(m_chScaleY);
	if (m_chScaleZ[0] != '\0') fScaleZ = (_float)atof(m_chScaleZ);
	(*m_ppTargetObject)->SetPosition(vec3(fPosX, fPosY, fPosZ));
	(*m_ppTargetObject)->SetRotation(vec3(fRotX, fRotY, fRotZ));
	(*m_ppTargetObject)->SetScale(vec3(fScaleX, fScaleY, fScaleZ));

	Text(" ");
	Text("Interaction Sound Setting:");
	Text(" ");
	Text("Light Setting(later):");
	//}
	//EndChild();
}

void MapEditorUIManager::RenderMeshList(_float screenX, _float screenY)
{
	Text(" ");
	SameLine(110.f); Text("Add Button");
	SameLine(300.f); Text("Size");

	for (int i = 0; i < m_vecMeshList.size(); ++i)
	{
		string meshId = m_vecMeshList[i];
		if (Button(meshId.c_str(), ImVec2(270.f, 0.f)))
		{
			if (nullptr != m_pScene)
			{
				vec3 vPos = vec3(0.f);
				vec3 vRot = vec3(0.f);
				_float fSize = 0.f;
				if (m_vecInputList[i][0] == '\0')
					fSize = 1.f;
				else
					fSize = stof(m_vecInputList[i]);
				if (0.f == fSize)
					fSize = 1.f;
				vec3 vScale = vec3(fSize);
				CGameObject* pGameObject = nullptr;
				pGameObject = BGObject::Create(m_pScene->GetSceneTag(), (_uint)LAYER_BACKGROUND, (_uint)OBJ_BACKGROUND
					, m_pScene->GetLayer((_uint)LAYER_BACKGROUND)
					, meshId
					, vPos, vRot, vScale);

				if (nullptr != pGameObject)
					m_pScene->AddGameObjectToLayer((_uint)LAYER_BACKGROUND, pGameObject);
			}
		}
		SameLine(290.f);
		SetNextItemWidth(50.f);
		stringstream ss;
		ss << "##text_" << meshId;
		InputText(ss.str().c_str(), m_vecInputList[i], sizeof(m_vecInputList[i]));
	}
}

void MapEditorUIManager::ConvertFloatToCharArray(char* dest, _float value)
{
	stringstream ss;
	ss << value;
	strcpy_s(dest, ss.str().length() + 1, ss.str().c_str());
}

