#include "MapEditorUIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "Enums.h"
#include "OpenGLDevice.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Scene.h"
#include "GameObject.h"
#include "Tree.h"
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
	: m_pScene(nullptr)
{
	m_vecMeshList.clear();
	m_vecTextList.clear();
}

MapEditorUIManager::~MapEditorUIManager()
{
}

void MapEditorUIManager::Destroy()
{
	m_vecMeshList.clear();

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
	SetNextWindowSize(ImVec2(400.f, 800.f));
	if (Begin("Pumpkins Map Editor", (bool*)0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		RenderMeshList();
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT MapEditorUIManager::Ready(CScene* pScene)
{
	m_pScene = pScene;

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
		m_vecTextList.push_back(iter->second);
	}

	return PK_NOERROR;
}

void MapEditorUIManager::RenderMeshList()
{
	for (int i = 0; i < m_vecMeshList.size(); ++i)
	{
		string meshId = m_vecMeshList[i];
		Text("Add");
		SameLine(50.f);
		SetNextItemWidth(50.f);
		stringstream ss;
		ss << "##text_" << meshId;
		InputText(ss.str().c_str(), (char*)m_vecTextList[i].c_str(), sizeof(m_vecTextList[i]));
		SameLine(110.f);

		if (Button(meshId.c_str(), ImVec2(270.f, 0.f)))
		{
			if (nullptr != m_pScene)
			{
				vec3 vPos = vec3(0.f);
				vec3 vRot = vec3(0.f);
				float fSize = m_vecTextList[i] == "" ? 1.f : stof(m_vecTextList[i]);
				vec3 vScale = vec3(fSize);
				CGameObject* pGameObject = nullptr;
				pGameObject = Tree::Create(m_pScene->GetSceneTag(), (_uint)LAYER_BACKGROUND, (_uint)OBJ_BACKGROUND
					, m_pScene->GetLayer((_uint)LAYER_BACKGROUND)
					, meshId
					, vPos, vRot, vScale);

				if (nullptr != pGameObject)
					m_pScene->AddGameObjectToLayer((_uint)LAYER_BACKGROUND, pGameObject);
			}
		}
	}
}

