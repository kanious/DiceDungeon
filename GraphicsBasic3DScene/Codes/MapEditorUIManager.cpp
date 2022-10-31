#include "MapEditorUIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "Enums.h"
#include "OpenGLDevice.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "BGObject.h"
#include "LightMaster.h"
#include "Light.h"
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
	: m_pScene(nullptr), m_ppTargetObject(nullptr), m_ppBGLayer(nullptr), m_pTargetLight(nullptr)
{
	m_vecMeshInfo.clear();

	ZeroMemory(m_chPos, sizeof(m_chPos));
	ZeroMemory(m_chRot, sizeof(m_chRot));
	ZeroMemory(m_chScale, sizeof(m_chScale));

	ZeroMemory(m_chLightPos, sizeof(m_chLightPos));
	ZeroMemory(m_chLightDir, sizeof(m_chLightDir));
	ZeroMemory(m_chLightDiff, sizeof(m_chLightDiff));
	ZeroMemory(m_chLightspec, sizeof(m_chLightspec));
	ZeroMemory(m_chLightambi, sizeof(m_chLightambi));
	ZeroMemory(m_chLightatten, sizeof(m_chLightatten));
	ZeroMemory(m_chLightparam1, sizeof(m_chLightparam1));
	ZeroMemory(m_chLightparam2, sizeof(m_chLightparam2));

	m_isPreviousZeroLock = false;
	m_isZeroLock = false;
	m_isPreviousZeroShow = true;
	m_isZeroShow = true;
	m_isPreviousZeroDebug = true;
	m_isZeroDebug = true;
	m_isPreviousZeroWire = false;
	m_isZeroWire = false;
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
	vector<MeshInfo>::iterator iter;
	for (iter = m_vecMeshInfo.begin(); iter != m_vecMeshInfo.end(); ++iter)
		delete iter->inputSize;
	m_vecMeshInfo.clear();

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
		//ImVec2 screenSize = GetWindowSize();

		RenderSceneDetailUI();
		RenderTargetDetailUI();
		RenderObjectList();
		RenderLightList();
	}
	End();

	SetNextWindowPos(ImVec2(COpenGLDevice::GetInstance()->GetWidthSize() - 360.f, 0.f));
	SetNextWindowSize(ImVec2(360.f, (_float)COpenGLDevice::GetInstance()->GetHeightSize()));
	if (Begin("Mesh List", (bool*)0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		RenderMeshList();
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT MapEditorUIManager::Ready(CScene* pScene, BGObject** ppTarget, CLayer** ppLayer)
{
	m_pScene = pScene;
	m_ppTargetObject = ppTarget;
	m_ppBGLayer = ppLayer;

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
		MeshInfo info;
		info.meshId = iter->first;
		char* inputText = new char[128];
		memset(inputText, 0, sizeof(*inputText));
		strcpy_s(inputText, iter->second.length() + 1, iter->second.c_str());
		info.inputSize = inputText;
		m_vecMeshInfo.push_back(info);
	}

	return PK_NOERROR;
}

void MapEditorUIManager::RenderSceneDetailUI()
{
	if (nullptr == m_pScene)
		return;

	Text("* Scene Detail Setting");
	Text("Data Path: "); SameLine(120.f); Text(m_pScene->GetDataPath().c_str());
	Text("Obj List File: "); SameLine(120.f); Text(m_pScene->GetObjListFileName().c_str());
	if (Button("Object List Save", ImVec2(190.f, 0.f)))
	{
		ObjListSave();
	}
	SameLine(200.f);
	if (Button("Object List Load", ImVec2(190.f, 0.f)))
	{
		ObjListLoad();
	}
}

void MapEditorUIManager::RenderTargetDetailUI()
{
	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 1));
	if (BeginChild("##TargetSetting", ImVec2(380.f, 150.f)))
	{
		Text("* Target Detail Setting");
		if (nullptr == *m_ppTargetObject)
		{
			Text("There is no object target.");
			PopStyleColor();
			EndChild();
			return;
		}

		Text(" ");
		SameLine(140.f);
		_bool isLock = (*m_ppTargetObject)->GetLock();
		Checkbox("Lock##Target", &isLock);
		(*m_ppTargetObject)->SetLock(isLock);

		SameLine(200.f);
		_bool isEnable = (*m_ppTargetObject)->GetEnable();
		Checkbox("Show##Target", &isEnable);
		(*m_ppTargetObject)->SetEnable(isEnable);

		SameLine(260.f);
		_bool isDebug = (*m_ppTargetObject)->GetDebug();
		Checkbox("Box##Target", &isDebug);
		(*m_ppTargetObject)->SetDebug(isDebug);

		SameLine(320.f);
		_bool isWire = (*m_ppTargetObject)->GetWireFrame();
		Checkbox("Wire##Target", &isWire);
		(*m_ppTargetObject)->SetWireFrame(isWire);

		Text("Mesh ID:"); SameLine(80.f); Text((*m_ppTargetObject)->GetMeshID().c_str());
		Text(" ");

		vec3 vPos = (*m_ppTargetObject)->GetPosition();
		ConvertFloatToCharArray(m_chPos[0], vPos.x);
		ConvertFloatToCharArray(m_chPos[1], vPos.y);
		ConvertFloatToCharArray(m_chPos[2], vPos.z);
		vec3 vRot = (*m_ppTargetObject)->GetRotation();
		ConvertFloatToCharArray(m_chRot[0], vRot.x);
		ConvertFloatToCharArray(m_chRot[1], vRot.y);
		ConvertFloatToCharArray(m_chRot[2], vRot.z);
		vec3 vScale = (*m_ppTargetObject)->GetScale();
		ConvertFloatToCharArray(m_chScale[0], vScale.x);
		ConvertFloatToCharArray(m_chScale[1], vScale.y);
		ConvertFloatToCharArray(m_chScale[2], vScale.z);

		Text("Position");
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##PosX", m_chPos[0], sizeof(m_chPos[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##PosY", m_chPos[1], sizeof(m_chPos[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##PosZ", m_chPos[2], sizeof(m_chPos[2]));

		Text("Rotation");
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##RotX", m_chRot[0], sizeof(m_chRot[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##RotY", m_chRot[1], sizeof(m_chRot[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##RotZ", m_chRot[2], sizeof(m_chRot[2]));

		Text("Scale");
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##ScaleX", m_chScale[0], sizeof(m_chScale[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##ScaleY", m_chScale[1], sizeof(m_chScale[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##ScaleZ", m_chScale[2], sizeof(m_chScale[2]));

		_float fPosX = vPos.x; _float fPosY = vPos.y; _float fPosZ = vPos.z;
		_float fRotX = vRot.x; _float fRotY = vRot.y; _float fRotZ = vRot.z;
		_float fScaleX = vScale.x; _float fScaleY = vScale.y; _float fScaleZ = vScale.z;
		if (m_chPos[0][0] != '\0') fPosX = (_float)atof(m_chPos[0]);
		if (m_chPos[1][0] != '\0') fPosY = (_float)atof(m_chPos[1]);
		if (m_chPos[2][0] != '\0') fPosZ = (_float)atof(m_chPos[2]);
		if (m_chRot[0][0] != '\0') fRotX = (_float)atof(m_chRot[0]);
		if (m_chRot[1][0] != '\0') fRotY = (_float)atof(m_chRot[1]);
		if (m_chRot[2][0] != '\0') fRotZ = (_float)atof(m_chRot[2]);
		if (m_chScale[0][0] != '\0') fScaleX = (_float)atof(m_chScale[0]);
		if (m_chScale[1][0] != '\0') fScaleY = (_float)atof(m_chScale[1]);
		if (m_chScale[2][0] != '\0') fScaleZ = (_float)atof(m_chScale[2]);
		(*m_ppTargetObject)->SetPosition(vec3(fPosX, fPosY, fPosZ));
		(*m_ppTargetObject)->SetRotation(vec3(fRotX, fRotY, fRotZ));
		(*m_ppTargetObject)->SetScale(vec3(fScaleX, fScaleY, fScaleZ));

		//Text(" ");
		//Text("Interaction Sound Setting:");
	}
	PopStyleColor();
	EndChild();
}

void MapEditorUIManager::RenderObjectList()
{
	if (nullptr == m_ppBGLayer || nullptr == *m_ppBGLayer)
		return;

	if (BeginChild("##ObjectLists", ImVec2(380.f, 400.f)))
	{
		Text("* Object Lists");
		Text(" ");
		SameLine(140.f);
		Checkbox("Lock##Zero", &m_isZeroLock);
		if (m_isPreviousZeroLock != m_isZeroLock)
		{
			m_isPreviousZeroLock = m_isZeroLock;
			list<CGameObject*>* listObj = (*m_ppBGLayer)->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetLock(m_isZeroLock);
		}

		SameLine(200.f);
		Checkbox("Show##Zero", &m_isZeroShow);
		if (m_isPreviousZeroShow != m_isZeroShow)
		{
			m_isPreviousZeroShow = m_isZeroShow;
			list<CGameObject*>* listObj = (*m_ppBGLayer)->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetEnable(m_isZeroShow);
		}

		SameLine(260.f);
		Checkbox("Box##Zero", &m_isZeroDebug);
		if (m_isPreviousZeroDebug != m_isZeroDebug)
		{
			m_isPreviousZeroDebug = m_isZeroDebug;
			list<CGameObject*>* listObj = (*m_ppBGLayer)->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetDebug(m_isZeroDebug);
		}

		SameLine(310.f);
		Checkbox("Wire##Zero", &m_isZeroWire);
		if (m_isPreviousZeroWire != m_isZeroWire)
		{
			m_isPreviousZeroWire = m_isZeroWire;
			list<CGameObject*>* listObj = (*m_ppBGLayer)->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetWireFrame(m_isZeroWire);
		}

		_uint index = 0;
		list<CGameObject*>* listObj = (*m_ppBGLayer)->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
		{
			stringstream ss;
			ss << (*iter)->GetName() << "##Pick_" << index;
			if (Button(ss.str().c_str(), ImVec2(120.f, 0.f)))
			{
				if (nullptr != *m_ppTargetObject)
					(*m_ppTargetObject)->SetSelected(false);
				*m_ppTargetObject = dynamic_cast<BGObject*>(*iter);
				(*m_ppTargetObject)->SetSelected(true);
			}

			SameLine(140.f);
			_bool isLock = (*iter)->GetLock();
			ss.str("");  ss << "Lock##" << index;
			Checkbox(ss.str().c_str(), &isLock);
			(*iter)->SetLock(isLock);

			SameLine(200.f);
			_bool isEnable = (*iter)->GetEnable();
			ss.str("");  ss << "Show##" << index;
			Checkbox(ss.str().c_str(), &isEnable);
			(*iter)->SetEnable(isEnable);

			SameLine(260.f);
			_bool isDebug = (*iter)->GetDebug();
			ss.str("");  ss << "Box##" << index;
			Checkbox(ss.str().c_str(), &isDebug);
			(*iter)->SetDebug(isDebug);

			SameLine(310.f);
			_bool isWire = (*iter)->GetWireFrame();
			ss.str("");  ss << "Wire##" << index;
			Checkbox(ss.str().c_str(), &isWire);
			(*iter)->SetWireFrame(isWire);

			++index;
		}
	}
	EndChild();
}

void MapEditorUIManager::RenderLightList()
{
	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1, 0.1, 0.1, 1));
	if (BeginChild("##LightSetting", ImVec2(380.f, 220.f)))
	{
		Text("* Light Target Detail Setting");
		if (nullptr == m_pTargetLight)
		{
			Text("There is no light target.");
			Text(" ");
		}
		else
		{
			CLight::cLightInfo* pInfo = m_pTargetLight->GetLightInfo();
			Text(pInfo->name.c_str());

			// position
			vec4 value = pInfo->position;
			ConvertFloatToCharArray(m_chLightPos[0], value.x);
			ConvertFloatToCharArray(m_chLightPos[1], value.y);
			ConvertFloatToCharArray(m_chLightPos[2], value.z);
			ConvertFloatToCharArray(m_chLightPos[3], value.w);

			Text("Position");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightPosX", m_chLightPos[0], sizeof(m_chLightPos[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightPosY", m_chLightPos[1], sizeof(m_chLightPos[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightPosZ", m_chLightPos[2], sizeof(m_chLightPos[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightPosW", m_chLightPos[3], sizeof(m_chLightPos[3]));

			_float fPosX = value.x; _float fPosY = value.y; _float fPosZ = value.z; _float fPosW = value.w;
			if (m_chLightPos[0][0] != '\0') fPosX = (_float)atof(m_chLightPos[0]);
			if (m_chLightPos[1][0] != '\0') fPosY = (_float)atof(m_chLightPos[1]);
			if (m_chLightPos[2][0] != '\0') fPosZ = (_float)atof(m_chLightPos[2]);
			if (m_chLightPos[3][0] != '\0') fPosW = (_float)atof(m_chLightPos[3]);
			pInfo->position = vec4(fPosX, fPosY, fPosZ, fPosW);


			//direction
			value = pInfo->direction;
			ConvertFloatToCharArray(m_chLightDir[0], value.x);
			ConvertFloatToCharArray(m_chLightDir[1], value.y);
			ConvertFloatToCharArray(m_chLightDir[2], value.z);
			ConvertFloatToCharArray(m_chLightDir[3], value.w);

			Text("Direction");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightDirX", m_chLightDir[0], sizeof(m_chLightDir[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightDirY", m_chLightDir[1], sizeof(m_chLightDir[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightDirZ", m_chLightDir[2], sizeof(m_chLightDir[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightDirW", m_chLightDir[3], sizeof(m_chLightDir[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightDir[0][0] != '\0') fPosX = (_float)atof(m_chLightDir[0]);
			if (m_chLightDir[1][0] != '\0') fPosY = (_float)atof(m_chLightDir[1]);
			if (m_chLightDir[2][0] != '\0') fPosZ = (_float)atof(m_chLightDir[2]);
			if (m_chLightDir[3][0] != '\0') fPosW = (_float)atof(m_chLightDir[3]);
			pInfo->direction = vec4(fPosX, fPosY, fPosZ, fPosW);


			//diffuse
			value = pInfo->diffuse;
			ConvertFloatToCharArray(m_chLightDiff[0], value.x);
			ConvertFloatToCharArray(m_chLightDiff[1], value.y);
			ConvertFloatToCharArray(m_chLightDiff[2], value.z);
			ConvertFloatToCharArray(m_chLightDiff[3], value.w);

			Text("Diffuse");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightDiffX", m_chLightDiff[0], sizeof(m_chLightDiff[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightDiffY", m_chLightDiff[1], sizeof(m_chLightDiff[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightDiffZ", m_chLightDiff[2], sizeof(m_chLightDiff[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightDiffW", m_chLightDiff[3], sizeof(m_chLightDiff[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightDiff[0][0] != '\0') fPosX = (_float)atof(m_chLightDiff[0]);
			if (m_chLightDiff[1][0] != '\0') fPosY = (_float)atof(m_chLightDiff[1]);
			if (m_chLightDiff[2][0] != '\0') fPosZ = (_float)atof(m_chLightDiff[2]);
			if (m_chLightDiff[3][0] != '\0') fPosW = (_float)atof(m_chLightDiff[3]);
			pInfo->diffuse = vec4(fPosX, fPosY, fPosZ, fPosW);


			//specular
			value = pInfo->specular;
			ConvertFloatToCharArray(m_chLightspec[0], value.x);
			ConvertFloatToCharArray(m_chLightspec[1], value.y);
			ConvertFloatToCharArray(m_chLightspec[2], value.z);
			ConvertFloatToCharArray(m_chLightspec[3], value.w);

			Text("Specular");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightSpecX", m_chLightspec[0], sizeof(m_chLightspec[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightSpecY", m_chLightspec[1], sizeof(m_chLightspec[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightSpecZ", m_chLightspec[2], sizeof(m_chLightspec[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightSpecW", m_chLightspec[3], sizeof(m_chLightspec[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightspec[0][0] != '\0') fPosX = (_float)atof(m_chLightspec[0]);
			if (m_chLightspec[1][0] != '\0') fPosY = (_float)atof(m_chLightspec[1]);
			if (m_chLightspec[2][0] != '\0') fPosZ = (_float)atof(m_chLightspec[2]);
			if (m_chLightspec[3][0] != '\0') fPosW = (_float)atof(m_chLightspec[3]);
			pInfo->specular = vec4(fPosX, fPosY, fPosZ, fPosW);


			//ambient
			value = pInfo->ambient;
			ConvertFloatToCharArray(m_chLightambi[0], value.x);
			ConvertFloatToCharArray(m_chLightambi[1], value.y);
			ConvertFloatToCharArray(m_chLightambi[2], value.z);
			ConvertFloatToCharArray(m_chLightambi[3], value.w);

			Text("Ambient");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightAmbiX", m_chLightambi[0], sizeof(m_chLightambi[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightAmbiY", m_chLightambi[1], sizeof(m_chLightambi[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightAmbiZ", m_chLightambi[2], sizeof(m_chLightambi[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightAmbiW", m_chLightambi[3], sizeof(m_chLightambi[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightambi[0][0] != '\0') fPosX = (_float)atof(m_chLightambi[0]);
			if (m_chLightambi[1][0] != '\0') fPosY = (_float)atof(m_chLightambi[1]);
			if (m_chLightambi[2][0] != '\0') fPosZ = (_float)atof(m_chLightambi[2]);
			if (m_chLightambi[3][0] != '\0') fPosW = (_float)atof(m_chLightambi[3]);
			pInfo->ambient = vec4(fPosX, fPosY, fPosZ, fPosW);


			//atten
			value = pInfo->atten;
			ConvertFloatToCharArray(m_chLightatten[0], value.x);
			ConvertFloatToCharArray(m_chLightatten[1], value.y);
			ConvertFloatToCharArray(m_chLightatten[2], value.z);
			ConvertFloatToCharArray(m_chLightatten[3], value.w);

			Text("Attenuation");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##LightAttenX", m_chLightatten[0], sizeof(m_chLightatten[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##LightAttenY", m_chLightatten[1], sizeof(m_chLightatten[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##LightAttenZ", m_chLightatten[2], sizeof(m_chLightatten[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##LightAttenW", m_chLightatten[3], sizeof(m_chLightatten[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightatten[0][0] != '\0') fPosX = (_float)atof(m_chLightatten[0]);
			if (m_chLightatten[1][0] != '\0') fPosY = (_float)atof(m_chLightatten[1]);
			if (m_chLightatten[2][0] != '\0') fPosZ = (_float)atof(m_chLightatten[2]);
			if (m_chLightatten[3][0] != '\0') fPosW = (_float)atof(m_chLightatten[3]);
			pInfo->atten = vec4(fPosX, fPosY, fPosZ, fPosW);


			//param1
			value = pInfo->param1;
			ConvertFloatToCharArray(m_chLightparam1[0], value.x);
			ConvertFloatToCharArray(m_chLightparam1[1], value.y);
			ConvertFloatToCharArray(m_chLightparam1[2], value.z);
			ConvertFloatToCharArray(m_chLightparam1[3], value.w);

			Text("Param1");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##Lightparam1X", m_chLightparam1[0], sizeof(m_chLightparam1[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##Lightparam1Y", m_chLightparam1[1], sizeof(m_chLightparam1[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##Lightparam1Z", m_chLightparam1[2], sizeof(m_chLightparam1[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##Lightparam1W", m_chLightparam1[3], sizeof(m_chLightparam1[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightparam1[0][0] != '\0') fPosX = (_float)atof(m_chLightparam1[0]);
			if (m_chLightparam1[1][0] != '\0') fPosY = (_float)atof(m_chLightparam1[1]);
			if (m_chLightparam1[2][0] != '\0') fPosZ = (_float)atof(m_chLightparam1[2]);
			if (m_chLightparam1[3][0] != '\0') fPosW = (_float)atof(m_chLightparam1[3]);
			pInfo->param1 = vec4(fPosX, fPosY, fPosZ, fPosW);


			//param2
			value = pInfo->param2;
			ConvertFloatToCharArray(m_chLightparam2[0], value.x);
			ConvertFloatToCharArray(m_chLightparam2[1], value.y);
			ConvertFloatToCharArray(m_chLightparam2[2], value.z);
			ConvertFloatToCharArray(m_chLightparam2[3], value.w);

			Text("Param2");
			SameLine(80.f); Text("X:"); SameLine(100.f);
			SetNextItemWidth(50); InputText("##Lightparam2X", m_chLightparam2[0], sizeof(m_chLightparam2[0]));
			SameLine(160.f); Text("Y:"); SameLine(180.f);
			SetNextItemWidth(50); InputText("##Lightparam2Y", m_chLightparam2[1], sizeof(m_chLightparam2[1]));
			SameLine(240.f); Text("Z:"); SameLine(260.f);
			SetNextItemWidth(50); InputText("##Lightparam2Z", m_chLightparam2[2], sizeof(m_chLightparam2[2]));
			SameLine(320.f); Text("W:"); SameLine(340.f);
			SetNextItemWidth(50); InputText("##Lightparam2W", m_chLightparam2[3], sizeof(m_chLightparam2[3]));

			fPosX = 0; fPosY = 0; fPosZ = 0; fPosW = 0;
			if (m_chLightparam2[0][0] != '\0') fPosX = (_float)atof(m_chLightparam2[0]);
			if (m_chLightparam2[1][0] != '\0') fPosY = (_float)atof(m_chLightparam2[1]);
			if (m_chLightparam2[2][0] != '\0') fPosZ = (_float)atof(m_chLightparam2[2]);
			if (m_chLightparam2[3][0] != '\0') fPosW = (_float)atof(m_chLightparam2[3]);
			pInfo->param2 = vec4(fPosX, fPosY, fPosZ, fPosW);
		}
	}
	PopStyleColor();
	EndChild();

	Text("* Light Lists");
	_int index = 0;
	unordered_map<string, CLight*>* pLightMap = CLightMaster::GetInstance()->GetLightMap();
	unordered_map<string, CLight*>::iterator iter;
	for (iter = pLightMap->begin(); iter != pLightMap->end(); ++iter)
	{
		CLight* pLight = iter->second;

		stringstream ss;
		ss << iter->first << "##PickLight_" << index;
		if (Button(ss.str().c_str(), ImVec2(140.f, 0.f)))
		{
			m_pTargetLight = iter->second;
		}

		++index;
	}
}

void MapEditorUIManager::RenderMeshList()
{
	Text(" ");
	SameLine(110.f); Text("Add Object");
	SameLine(300.f); Text("Size");

	for (int i = 0; i < m_vecMeshInfo.size(); ++i)
	{
		string meshId = m_vecMeshInfo[i].meshId;
		if (Button(meshId.c_str(), ImVec2(270.f, 0.f)))
		{
			if (nullptr != m_pScene)
			{
				vec3 vPos = vec3(0.f);
				vec3 vRot = vec3(0.f);
				_float fSize = 0.f;
				if (m_vecMeshInfo[i].inputSize[0] == '\0')
					fSize = 1.f;
				else
					fSize = stof(m_vecMeshInfo[i].inputSize);
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
		InputText(ss.str().c_str(), m_vecMeshInfo[i].inputSize, sizeof(m_vecMeshInfo[i].inputSize));
	}
}

void MapEditorUIManager::ConvertFloatToCharArray(char* dest, _float value)
{
	stringstream ss;
	ss << value;
	strcpy_s(dest, ss.str().length() + 1, ss.str().c_str());
}

void MapEditorUIManager::ObjListSave()
{
	if (nullptr != m_pScene)
		m_pScene->SaveBackgroundObjects();
}

void MapEditorUIManager::ObjListLoad()
{
	if (nullptr != m_pScene)
		m_pScene->LoadBackgroundObjects();

	if (nullptr != m_ppTargetObject)
	{
		if (nullptr != (*m_ppTargetObject))
		{
			(*m_ppTargetObject)->SetSelected(false);
			(*m_ppTargetObject) = nullptr;
		}
	}

	m_isPreviousZeroLock = false;
	m_isZeroLock = false;
	m_isPreviousZeroShow = true;
	m_isZeroShow = true;
	m_isPreviousZeroDebug = true;
	m_isZeroDebug = true;
	m_isPreviousZeroWire = false;
	m_isZeroWire = false;
}

