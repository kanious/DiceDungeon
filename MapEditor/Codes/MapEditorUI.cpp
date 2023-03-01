#include "MapEditorUI.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "GameMaster.h"
#include "Scene3D.h"
#include "UIManager.h"
#include "InputDevice.h"
#include "CollisionMaster.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Mesh.h"
#include "Layer.h"
#include "GameObject.h"
#include "Function.h"

#include <sstream>
#include <iomanip>

USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

MapEditorUI::MapEditorUI()
	: m_pScene(nullptr)
	, m_curLayerNameCombo("")
	, m_curMeshTypeCombo("")
	, m_pTarget(nullptr)
	, m_vPosPrev(vec3(0.f)), m_vRotPrev(vec3(0.f)), m_vScalePrev(vec3(0.f))
	, m_bScrollSet(false)
	, m_bSnap(false)
	, m_bLock(false), m_bLockPrev(false)
	, m_bShow(false), m_bShowPrev(false)
	, m_bBBox(false), m_bBBoxPrev(false)
	, m_bWire(false), m_bWirePrev(false)
	, m_bAlpha(false), m_bAlphaPrev(false)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pUIManager = UIManager::GetInstance();

	m_vecMeshInfos.clear();
	m_vecLayerNameCombo.clear();
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
	if (nullptr == m_pInputDevice || nullptr == m_pUIManager || nullptr == m_pScene)
		return;

	if (m_pUIManager->GetCursorIsOnTheUI())
		return;

	MoveTarget(dt);
	KeyCheck(dt);
}

void MapEditorUI::RenderUI()
{
	_float width = (_float)COpenGLDevice::GetInstance()->GetWidthSize();
	_float height = (_float)COpenGLDevice::GetInstance()->GetHeightSize();

	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(ImVec2(300.f, height));
	if (Begin("Pumpkins Map Editor", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_MenuBar))
	{
		SetMenuBar();
		
		Text("- Opened File Name :"); SameLine(150.f); Text(m_pScene->GetObjListFileName().c_str());
		Checkbox("Snap##Checkbox", &m_bSnap);

		Separator();
		Render_ObjectList(290.f, 310.f);
		
		Separator();
		Render_MeshList(290.f, 300.f);
	}
	End();

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
		
	}
	End();
}

void MapEditorUI::SetTarget(CGameObject* pObj)
{
	if (nullptr != m_pTarget)
	{
		m_pTarget->SetSelected(false);
		m_pTarget = nullptr;
		m_vPosPrev = vec3(0.f);
		m_vRotPrev = vec3(0.f);
		m_vScalePrev = vec3(0.f);
	}

	if (nullptr != pObj)
	{
		m_pTarget = pObj;
		pObj->SetSelected(true);
		m_bScrollSet = true;

		m_vPosPrev = pObj->GetPosition();
		m_vRotPrev = pObj->GetRotation();
		m_vScalePrev = pObj->GetScale();
	}
}

void MapEditorUI::ResetTarget()
{
	m_pTarget->SetPosition(m_vPosPrev);
	m_pTarget->SetRotation(m_vRotPrev);
	m_pTarget->SetScale(m_vScalePrev);

	SetTarget(nullptr);
}

void MapEditorUI::SetMenuBar()
{
	if (BeginMenuBar())
	{
		if (BeginMenu("File"))
		{
			if (MenuItem("New"))
			{
				if (nullptr != m_pScene)
					m_pScene->ResetAllLayers();
			}

			Separator();
			if (MenuItem("Load"))
			{
				if (nullptr != m_pScene)
					m_pScene->LoadBackgroundObjects();
			}
			if (MenuItem("Save"))
			{
				if (nullptr != m_pScene)
					m_pScene->SaveBackgroundObjects();
			}
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

		Text(GetLayerNameByTag((eLAYERTAG)vecLayers[i]->GetTag()).c_str());
	}

	// All Check boxes
	Text(" ");
	SameLine(170.f); Text("L");
	SameLine(193.f); Text("S");
	SameLine(218.f); Text("B");
	SameLine(242.f); Text("W");
	SameLine(267.f); Text("A");
	Text("All status change");
	SameLine(160.f); Checkbox("##LockAllCheckbox", &m_bLock);
	SameLine(185.f); Checkbox("##ShowAllCheckbox", &m_bShow);
	SameLine(210.f); Checkbox("##BBoxAllCheckbox", &m_bBBox);
	SameLine(235.f); Checkbox("##WireAllCheckbox", &m_bWire);
	SameLine(260.f); Checkbox("##AlphaAllCheckbox", &m_bAlpha);

	if (m_bLock != m_bLockPrev)
	{
		m_bLockPrev = m_bLock;
		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetLock(m_bLock);
		}
	}
	if (m_bShow != m_bShowPrev)
	{
		m_bShowPrev = m_bShow;
		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetEnable(m_bShow);
		}
	}
	if (m_bBBox != m_bBBoxPrev)
	{
		m_bBBoxPrev = m_bBBox;
		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetDebug(m_bBBox);
		}
	}
	if (m_bWire != m_bWirePrev)
	{
		m_bWirePrev = m_bWire;
		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetWireFrame(m_bWire);
		}
	}
	if (m_bAlpha != m_bAlphaPrev)
	{
		m_bAlphaPrev = m_bAlpha;
		for (_uint i = 0; i < vecLayers.size(); ++i)
		{
			if (!vecLayers[i]->GetEnable())
				continue;

			list<CGameObject*>* listObj = vecLayers[i]->GetObjectList();
			list<CGameObject*>::iterator iter;
			for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				(*iter)->SetTransparency(m_bAlpha);
		}
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
				label.str("");
				label << (*iter)->GetObjName() << "##button_" << index;

				if ((*iter)->GetSelected())
				{
					PushStyleColor(ImGuiCol_Button, ImVec4(0.48f, 0.72f, 0.89f, 0.49f));

					if (m_bScrollSet)
					{
						SetScrollHereY();
						m_bScrollSet = false;
					}
				}
				else
				{
					PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.75f));
				}

				if (Button(label.str().c_str()))
				{
					SetTarget((*iter));
				}

				PopStyleColor();

				_bool bLock = (*iter)->GetLock();
				label.str(""); label << "##LockCheckbox_" << index;
				SameLine(152.f); Checkbox(label.str().c_str(), &bLock);
				(*iter)->SetLock(bLock);

				_bool bShow = (*iter)->GetEnable();
				label.str(""); label << "##ShowCheckbox_" << index;
				SameLine(177.f); Checkbox(label.str().c_str(), &bShow);
				(*iter)->SetEnable(bShow);

				_bool bBBox = (*iter)->GetDebug();
				label.str(""); label << "##BBoxCheckbox_" << index;
				SameLine(202.f); Checkbox(label.str().c_str(), &bBBox);
				(*iter)->SetDebug(bBBox);

				_bool bWire = (*iter)->GetWireFrame();
				label.str(""); label << "##WireCheckbox_" << index;
				SameLine(227.f); Checkbox(label.str().c_str(), &bWire);
				(*iter)->SetWireFrame(bWire);

				_bool bAlpha = (*iter)->GetTransparency();
				label.str(""); label << "##AlphaCheckbox_" << index;
				SameLine(252.f); Checkbox(label.str().c_str(), &bAlpha);
				(*iter)->SetTransparency(bAlpha);

				++index;
			}
		}

	}
	EndChild();
}

void MapEditorUI::Render_MeshList(_float childX, _float childY)
{
	Text(" * MESH LIST");
	SameLine(170.f); SetNextItemWidth(100.f);
	if (BeginCombo("##LayerNameCombo", m_curLayerNameCombo, ImGuiComboFlags_None))
	{
		for (_uint i = 0; i < m_vecLayerNameCombo.size(); ++i)
		{
			_bool isSelected = m_curLayerNameCombo == m_vecLayerNameCombo[i];
			if (Selectable(m_vecLayerNameCombo[i], isSelected))
			{
				m_curLayerNameCombo = m_vecLayerNameCombo[i];
			}
		}
		
		EndCombo();
	}

	SetNextItemWidth(60.f);
	if (BeginCombo("##MeshTypeCombo", m_curMeshTypeCombo, ImGuiComboFlags_None))
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

	SameLine(90.f); Text("Mesh ID");
	SameLine(250); Text("Size");

	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.09f, 0.09f, 0.15f, 0.0f));
	if (BeginChild("##MeshList", ImVec2(childX, childY)))
	{
		stringstream label;
		string meshId;
		for (_uint i = 0; i < m_vecMeshInfos.size(); ++i)
		{
			MeshInfo info = m_vecMeshInfos[i];
			meshId = info.meshId;
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
				vec3 vPos = vec3(0.f);
				vec3 vRot = vec3(0.f);
				_float fSize = 0.f;
				if (m_vecMeshInfos[i].inputSize[0] == '\0')
					fSize = 1.f;
				else
					fSize = stof(m_vecMeshInfos[i].inputSize);
				if (0.f == fSize)
					fSize = 1.f;
				vec3 vScale = vec3(fSize);

				eLAYERTAG layerTag = GetLayerTagByName(m_curLayerNameCombo);
				m_pScene->AddGameObject(layerTag, meshId, vPos, vRot, vScale);
			}
			SameLine(50.f); Text(info.meshId.c_str());
			SameLine(235.f); SetNextItemWidth(35.f);
			label.str("");
			label << "##inputMeshSize_" << info.meshId;
			InputText(label.str().c_str(), info.inputSize, sizeof(info.inputSize));
		}
	}
	EndChild();
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

void MapEditorUI::MoveTarget(const _float& dt)
{
	if (nullptr == m_pTarget)
		return;

	vec3 vCameraPos = m_pScene->GetCameraPos();
	vec3 vDir = m_pInputDevice->GetMouseWorldCoord();
	vec3 vDest = vec3(0.f);
	if (CCollisionMaster::GetInstance()->IntersectRayToVirtualPlane(1000.f, vCameraPos, vDir, vDest))
	{
		vec3 vPos = m_pTarget->GetPosition();
		vDest.y = vPos.y;

		//if (!strcmp("floor", m_pTargetObject->GetMeshType().c_str()))
		if (m_bSnap)
		{
			if (0 > vDest.x)
				vDest.x = (_int)((vDest.x - 5) / 10) * 10;
			else
				vDest.x = (_int)((vDest.x + 5) / 10) * 10;

			if (0 > vDest.z)
				vDest.z = (_int)((vDest.z - 5) / 10) * 10;
			else
				vDest.z = (_int)((vDest.z + 5) / 10) * 10;

			//if (!strcmp("wall", m_pTarget->GetMeshType().c_str()))
			//{
			//	_float fY = m_pTarget->GetRotationY();

			//	if (fY == 0.f)
			//	{
			//		vDest.z += 5.f;
			//	}
			//	else if (fY == 90.f)
			//	{
			//		vDest.x -= 5.f;
			//	}
			//	else if (fY == 180.f)
			//	{
			//		vDest.z -= 5.f;
			//	}
			//	else
			//	{
			//		vDest.x += 5.f;
			//	}
			//}
		}

		m_pTarget->SetPosition(vDest);
	}
}

void MapEditorUI::KeyCheck(const _float& dt)
{
	// Object Picking
	static _bool isMouseLBClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isMouseLBClicked)
		{
			isMouseLBClicked = true;

			if (nullptr == m_pTarget)
			{
				SetTarget(m_pScene->GetTarget());
			}
			else
			{
				SetTarget(nullptr);
			}
		}
	}
	else
		isMouseLBClicked = false;

	// Object cancel
	static _bool isMouseRBClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_2))
	{
		if (!isMouseRBClicked)
		{
			isMouseRBClicked = true;

			if (nullptr != m_pTarget)
			{
				ResetTarget();
			}
		}
	}
	else
		isMouseRBClicked = false;

	// Object Delete
	static _bool isDelDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_DELETE))
	{
		if (!isDelDown)
		{
			isDelDown = true;

			if (nullptr != m_pTarget)
			{
				m_pTarget->SetDead(true);
				m_pTarget->SetSelected(false);
				m_pTarget = nullptr;
			}
		}
	}
	else
		isDelDown = false;
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

	// Set LayerNameComboList
	m_vecLayerNameCombo.push_back("Layer_camera");
	m_vecLayerNameCombo.push_back("Layer_static");
	m_vecLayerNameCombo.push_back("Layer_interactive");
	m_vecLayerNameCombo.push_back("Layer_character");
	m_vecLayerNameCombo.push_back("Layer_enemy");
	m_vecLayerNameCombo.push_back("Layer_ui");
	m_vecLayerNameCombo.push_back("Layer_event");
	m_curLayerNameCombo = "Layer_static";

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
