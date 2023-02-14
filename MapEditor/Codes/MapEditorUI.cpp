#include "MapEditorUI.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "GameMaster.h"
#include "Scene3D.h"
#include "UIManager.h"
#include "InputDevice.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Mesh.h"
#include "Layer.h"
#include "GameObject.h"

#include <sstream>
#include <iomanip>

USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

MapEditorUI::MapEditorUI()
	: m_pScene(nullptr), m_curMeshTypeCombo(""), m_pTarget(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance();

	m_vecMeshInfos.clear();
	m_vecMeshTypeCombo.clear();

	ZeroMemory(m_chPos, sizeof(m_chPos));
	ZeroMemory(m_chRot, sizeof(m_chRot));
	ZeroMemory(m_chScale, sizeof(m_chScale));
}

MapEditorUI::~MapEditorUI()
{
}

void MapEditorUI::Destroy()
{
	SafeDestroy(m_pInputDevice);

	for (_uint i = 0; i < m_vecMeshInfos.size(); ++i)
		delete m_vecMeshInfos[i].inputSize;
	m_vecMeshInfos.clear();
}

void MapEditorUI::Update(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pUIManager)
		return;

	if (m_pUIManager->GetCursorIsOnTheUI())
		return;

	// Object Picking
	//static _bool isMouseClicked = false;
	//if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	//{
	//	if (!isMouseClicked)
	//	{
	//		isMouseClicked = true;

	//		if (nullptr != m_pTarget)
	//		{
	//			m_pTarget->SetSelected(false);
	//			m_pTarget = nullptr;
	//		}

			//if (nullptr != m_pBGLayer)
			//{
			//	vec3 vCameraPos = m_pScene->GetCameraPos();
			//	//vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
			//	vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

			//	vector<CGameObject*> vecPicking;
			//	list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
			//	list<CGameObject*>::iterator iter;
			//	for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			//	{
			//		if ((*iter)->GetLock())
			//			continue;

			//		if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
			//			(*iter)->GetBoundingBox(),
			//			(*iter)->GetTransform(), vCameraPos, vDir))
			//		{
			//			vecPicking.push_back(*iter);
			//		}
			//	}

			//	if (vecPicking.size() > 0)
			//	{
			//		_float distanceMin = FLT_MAX; _int index = 0;
			//		for (int i = 0; i < vecPicking.size(); ++i)
			//		{
			//			_float dis = distance(vCameraPos, vecPicking[i]->GetPosition());
			//			if (dis < distanceMin)
			//			{
			//				distanceMin = dis;
			//				index = i;
			//			}
			//		}
			//		m_pTargetObject = dynamic_cast<BGObject*>(vecPicking[index]);
			//		m_pTargetObject->SetSelected(true);
			//	}
			//}

	//	}
	//}
	//else
	//	isMouseClicked = false;
}

void MapEditorUI::RenderUI()
{
	_float width = (_float)COpenGLDevice::GetInstance()->GetWidthSize();
	_float height = (_float)COpenGLDevice::GetInstance()->GetHeightSize();

	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(ImVec2(285.f, height));
	if (Begin("Pumpkins Map Editor", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_MenuBar))
	{
		SetMenuBar();
		
		Text("- Opened File Name :"); SameLine(150.f); Text(m_pScene->GetObjListFileName().c_str());
		
		Separator();
		Render_ObjectList(275.f, 310.f);
		
		Separator();
		Render_MeshList(275.f, 425.f);

		End();
	}

	if (nullptr == m_pTarget)
		return;

	SetNextWindowPos(ImVec2(width - 400.f, 0.f));
	SetNextWindowSize(ImVec2(400.f, height));
	if (Begin("Inspector", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		Render_Inspector();
		End();
	}
}

void MapEditorUI::SetMenuBar()
{
	if (BeginMenuBar())
	{
		if (BeginMenu("File"))
		{
			MenuItem("New");

			Separator();
			MenuItem("Load");
			MenuItem("Save");
			MenuItem("Save as...");

			Separator();
			if (MenuItem("Exit"))
			{
				CGameMaster::GetInstance()->SetGameClose(true);
			}

			ImGui::EndMenu();
		}

		EndMenuBar();
	}
}

void MapEditorUI::Render_ObjectList(_float childX, _float childY)
{
	Text(" * OBJECT LIST");

	// Layer Options
	stringstream label;
	vector<CLayer*>& vecLayers = m_pScene->GetLayers();
	for (_uint i = 0; i < vecLayers.size(); ++i)
	{
		if (0 == i % 2)
		{
			Text(" ");
			SameLine(5.f);
		}
		else
		{
			SameLine(155.f);
		}

		label.str("");
		label << "##layer_enable_checkbox_" << i;
		_bool isEnable = vecLayers[i]->GetEnable();
		Checkbox(label.str().c_str(), &isEnable);
		vecLayers[i]->SetEnable(isEnable);

		if (0 == i % 2)
			SameLine(30.f);
		else
			SameLine(180.f);

		Text(GetLayerTagByIndex((eLAYERTAG)vecLayers[i]->GetTag()).c_str());
	}

	// Object Lists
	if (BeginChild("##ObjectList", ImVec2(childX, childY)))
	{
		_uint index = 0;

		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			{
				if ((*iter)->GetSelected())
				{
					PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 0.f, 1.0f));
					Text((*iter)->GetObjName().c_str());
					PopStyleColor();
				}
				else
					Text((*iter)->GetObjName().c_str());
			}
		}

		EndChild();
	}
}

void MapEditorUI::Render_MeshList(_float childX, _float childY)
{
	Text(" * MESH LIST");
	SameLine(170.f); SetNextItemWidth(100.f);
	if (BeginCombo("##MeshTypeList", m_curMeshTypeCombo, ImGuiComboFlags_None))
	{
		for (_uint i = 0; i < m_vecMeshTypeCombo.size(); ++i)
		{
			_bool isSelected = m_curMeshTypeCombo == m_vecMeshTypeCombo[i];
			if (Selectable(m_vecMeshTypeCombo[i], isSelected))
			{
				m_curMeshTypeCombo = m_vecMeshTypeCombo[i];
			}
		}
		
		EndCombo();
	}

	Text(" ");
	SameLine(90.f); Text("Mesh ID");
	SameLine(235); Text("Size");

	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.15f, 0.0f));
	if (BeginChild("##MeshList", ImVec2(childX, childY)))
	{
		stringstream label;
		string meshId;
		for (_uint i = 0; i < m_vecMeshInfos.size(); ++i)
		{
			MeshInfo info = m_vecMeshInfos[i];
			const char* meshType = info.meshType.c_str();
			if (!strcmp("", meshType))
				meshType = "etc";

			if (strcmp("All", m_curMeshTypeCombo))
			{
				if (strcmp(m_curMeshTypeCombo, meshType))
				{
					continue;
				}
			}

			label.str("");
			label << " + ##" << info.meshId;
			if (Button(label.str().c_str(), ImVec2(40.f, 0.f)))
			{

			}
			SameLine(50.f); Text(info.meshId.c_str());
			SameLine(220.f); SetNextItemWidth(35.f);
			label.str("");
			label << "##inputMeshSize_" << info.meshId;
			InputText(label.str().c_str(), info.inputSize, sizeof(info.inputSize));
		}

		EndChild();
	}
	PopStyleColor();
}

void MapEditorUI::Render_Inspector()
{
	Text(" ");
	SameLine(140.f);
	_bool isLock = m_pTarget->GetLock();
	Checkbox("##Lock_Target", &isLock);
	m_pTarget->SetLock(isLock);

	SameLine(200.f);
	_bool isEnable = m_pTarget->GetEnable();
	Checkbox("##Show_Target", &isEnable);
	m_pTarget->SetEnable(isEnable);

	SameLine(260.f);
	_bool isDebug = m_pTarget->GetDebug();
	Checkbox("##BBox_Target", &isDebug);
	m_pTarget->SetDebug(isDebug);

	SameLine(320.f);
	_bool isWire = m_pTarget->GetWireFrame();
	Checkbox("##WireTarget", &isWire);
	m_pTarget->SetWireFrame(isWire);

	Text("Object Name: "); SameLine(80.f); Text(m_pTarget->GetObjName().c_str());
	Text(" ");

	vec3 vPos = m_pTarget->GetPosition();
	ConvertFloatToCharArray(m_chPos[0], vPos.x);
	ConvertFloatToCharArray(m_chPos[1], vPos.y);
	ConvertFloatToCharArray(m_chPos[2], vPos.z);
	vec3 vRot = m_pTarget->GetRotation();
	ConvertFloatToCharArray(m_chRot[0], vRot.x);
	ConvertFloatToCharArray(m_chRot[1], vRot.y);
	ConvertFloatToCharArray(m_chRot[2], vRot.z);
	vec3 vScale = m_pTarget->GetScale();
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
	m_pTarget->SetPosition(vec3(fPosX, fPosY, fPosZ));
	m_pTarget->SetRotation(vec3(fRotX, fRotY, fRotZ));
	m_pTarget->SetScale(vec3(fScaleX, fScaleY, fScaleZ));
}

void MapEditorUI::KeyCheck(const _float& dt)
{

}

string MapEditorUI::GetLayerTagByIndex(eLAYERTAG tag)
{
	string layerTag = "";
	switch (tag)
	{
	case LAYER_BACKGROUND_OBJECT:	layerTag = "Layer_background"; break;
	case LAYER_STATIC_OBJECT:		layerTag = "Layer_static"; break;
	case LAYER_INTERACTIVE_OBJECT:	layerTag = "Layer_interactive"; break;
	case LAYER_CHARACTER:			layerTag = "Layer_character"; break;
	case LAYER_ENEMY:				layerTag = "Layer_enemy"; break;
	case LAYER_UI:					layerTag = "Layer_ui"; break;
	case LAYER_EVENT_OBJECT:		layerTag = "Layer_event"; break;
	}
	return layerTag;
}

void MapEditorUI::ConvertFloatToCharArray(char* dest, _float value)
{
	stringstream ss;
	ss << value;
	strcpy_s(dest, ss.str().length() + 1, ss.str().c_str());
}

RESULT MapEditorUI::Ready(Scene3D* pScene)
{
	m_pScene = pScene;

	// Add MeshList
	vector<std::string>* pvecMesh = CComponentMaster::GetInstance()->GetMeshVector();
	for (_uint i = 0; i < pvecMesh->size(); ++i)
	{
		CComponent* pComp = CComponentMaster::GetInstance()->FindComponent((*pvecMesh)[i]);
		CMesh* pMesh = dynamic_cast<CMesh*>(pComp);
		if (nullptr != pMesh)
		{
			MeshInfo newInfo;

			newInfo.meshId = pMesh->GetTag();
			newInfo.meshType = pMesh->GetMeshType();

			char* inputText = new char[128];
			memset(inputText, 0, sizeof(*inputText));
			string initSize = pMesh->GetInitSize();
			strcpy_s(inputText, initSize.length() + 1, initSize.c_str());
			newInfo.inputSize = inputText;

			m_vecMeshInfos.push_back(newInfo);
		}
	}
	sort(m_vecMeshInfos.begin(), m_vecMeshInfos.end(), [](const MeshInfo& lhs, const MeshInfo& rhs)
		{ return lhs.meshId < rhs.meshId; });

	// Set MeshTypeComboList
	m_vecMeshTypeCombo.push_back("All");
	m_vecMeshTypeCombo.push_back("wall");
	m_vecMeshTypeCombo.push_back("floor");
	m_vecMeshTypeCombo.push_back("door");
	m_vecMeshTypeCombo.push_back("ceil");
	m_vecMeshTypeCombo.push_back("pillar");
	m_vecMeshTypeCombo.push_back("graveyard");
	m_vecMeshTypeCombo.push_back("skull");
	m_vecMeshTypeCombo.push_back("pumpkin");
	m_vecMeshTypeCombo.push_back("character");
	m_vecMeshTypeCombo.push_back("etc");
	m_curMeshTypeCombo = "All";

	return PK_NOERROR;
}

MapEditorUI* MapEditorUI::Create(Scene3D* pScene)
{
	MapEditorUI* pInstance = new MapEditorUI();
	if (PK_NOERROR != pInstance->Ready(pScene))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
