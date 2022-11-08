#include "MapEditorUI.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "Enums.h"
#include "OpenGLDevice.h"
#include "InputDevice.h"
#include "CollisionMaster.h"
#include "ComponentMaster.h"
#include "Component.h"
#include "Scene.h"
#include "SceneForest.h"
#include "Layer.h"
#include "GameObject.h"
#include "UIManager.h"
#include "DefaultCamera.h"
#include "BGObject.h"
#include "LightMaster.h"
#include "Light.h"
#include "PumpkinString.h"
#include "glm\vec3.hpp"
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>


USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

MapEditorUI::MapEditorUI()
	: m_pScene(nullptr), m_pTargetObject(nullptr)
	, m_pBGLayer(nullptr), m_pDefaultCamera(nullptr), m_pTargetLight(nullptr)
{
	m_pUIManager = UIManager::GetInstance();
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_pInputDevice->SetMouseSensitivity(0.05f);
	m_pString = PumpkinString::GetInstance(); m_pString->AddRefCnt();

	m_vecMeshInfo.clear();

	ZeroMemory(m_chPos, sizeof(m_chPos));
	ZeroMemory(m_chRot, sizeof(m_chRot));
	ZeroMemory(m_chScale, sizeof(m_chScale));
	ZeroMemory(m_chSound, sizeof(m_chSound));

	ZeroMemory(m_chLightName, sizeof(m_chLightName));
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
	m_isZeroDebug = false;
	m_isPreviousZeroWire = false;
	m_isZeroWire = false;
}

MapEditorUI::~MapEditorUI()
{
}

void MapEditorUI::Destroy()
{
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pString);

	vector<MeshInfo>::iterator iter;
	for (iter = m_vecMeshInfo.begin(); iter != m_vecMeshInfo.end(); ++iter)
		delete iter->inputSize;
	m_vecMeshInfo.clear();

	m_pScene = nullptr;
	m_pTargetObject = nullptr;
	m_pBGLayer = nullptr;
	m_pDefaultCamera = nullptr;
	m_pTargetLight = nullptr;

	ZeroMemory(m_chPos, sizeof(m_chPos));
	ZeroMemory(m_chRot, sizeof(m_chRot));
	ZeroMemory(m_chScale, sizeof(m_chScale));
	ZeroMemory(m_chSound, sizeof(m_chSound));

	ZeroMemory(m_chLightName, sizeof(m_chLightName));
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

void MapEditorUI::Update(const _float& dt)
{
	KeyCheck(dt);
}

void MapEditorUI::RenderUI()
{
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(ImVec2(400.f, (_float)COpenGLDevice::GetInstance()->GetHeightSize()));
	stringstream ss;
	ss << m_pString->GetString("TEXT_32") << "##Pumpkins Map Editor";
	if (Begin(ss.str().c_str(), (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		if (BeginTabBar("##Editor"))
		{
			ss.str("");
			ss << m_pString->GetString("TEXT_33") << "##ObjectTab";
			if (BeginTabItem(ss.str().c_str(), (bool*)0, ImGuiTabItemFlags_None))
			{
				ImVec2 screenSize = GetWindowSize();

				RenderSceneDetailUI();
				RenderObjectTargetDetailUI();
				RenderObjectList(screenSize.x, screenSize.y);

				EndTabItem();
			}
			
			ss.str("");
			ss << m_pString->GetString("TEXT_34") << "##LightTab";
			if (BeginTabItem(ss.str().c_str(), (bool*)0, ImGuiTabItemFlags_None))
			{
				RenderLightDetailUI();
				RenderLightTargetDetailUI();
				RenderLightList();

				EndTabItem();
			}
			
			EndTabBar();
		}
	}
	End();

	SetNextWindowPos(ImVec2(COpenGLDevice::GetInstance()->GetWidthSize() - 360.f, 0.f));
	SetNextWindowSize(ImVec2(360.f, (_float)COpenGLDevice::GetInstance()->GetHeightSize()));
	ss.str("");
	ss << m_pString->GetString("TEXT_35") << "##Mesh List";
	if (Begin(ss.str().c_str(), (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		RenderMeshList();
	}
	End();
}

RESULT MapEditorUI::Ready(CScene* pScene)
{
	if (nullptr != pScene)
	{
		m_pScene = pScene;
		m_pBGLayer = pScene->GetLayer((_uint)LAYER_BACKGROUND);
		m_pDefaultCamera = dynamic_cast<SceneForest*>(pScene)->GetDefaultCamera();
	}

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

void MapEditorUI::KeyCheck(const _float& dt)
{
	if (nullptr == m_pInputDevice || nullptr == m_pUIManager)
		return;

	if (m_pUIManager->GetUIOpened(UIManager::UI_SYSTEM_MENU_WINDOW) || m_pUIManager->GetCursorIsOnTheUI())
		goto NextCheck;

	// Object Picking
	static _bool isMouseClicked = false;
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
	{
		if (!isMouseClicked)
		{
			isMouseClicked = true;

			if (nullptr != m_pTargetObject)
			{
				m_pTargetObject->SetSelected(false);
				m_pTargetObject = nullptr;
			}

			if (nullptr != m_pBGLayer && nullptr != m_pDefaultCamera)
			{
				vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
				vec3 vDir = m_pInputDevice->GetMouseWorldCoord();

				vector<CGameObject*> vecPicking;
				list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
				list<CGameObject*>::iterator iter;
				for (iter = listObj->begin(); iter != listObj->end(); ++iter)
				{
					if ((*iter)->GetLock())
						continue;

					if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
						(*iter)->GetBoundingBox_AABB(),
						(*iter)->GetTransform(), vCameraPos, vDir))
					{
						vecPicking.push_back(*iter);
					}
				}

				if (vecPicking.size() > 0)
				{
					_float distanceMin = FLT_MAX; _int index = 0;
					for (int i = 0; i < vecPicking.size(); ++i)
					{
						_float dis = distance(vCameraPos, vecPicking[i]->GetPosition());
						if (dis < distanceMin)
						{
							distanceMin = dis;
							index = i;
						}
					}
					m_pTargetObject = dynamic_cast<BGObject*>(vecPicking[index]);
					m_pTargetObject->SetSelected(true);
				}
			}

		}
	}
	else
		isMouseClicked = false;

	// Object Moving
	if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_2))
	{
		if (nullptr != m_pDefaultCamera &&
			nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
			vec3 vDir = m_pInputDevice->GetMouseWorldCoord();
			vec3 vDest = vec3(0.f);
			if (CCollisionMaster::GetInstance()->IntersectRayToVirtualPlane(1000.f, vCameraPos, vDir, vDest))
			{
				vec3 vPos = m_pTargetObject->GetPosition();
				vDest.y = vPos.y;
				m_pTargetObject->SetPosition(vDest);
			}
		}
	}

	// Object Rotating Y Axis
	if (nullptr != m_pTargetObject)
	{
		vec2 vScroll = m_pInputDevice->GetMouseScrollDistance();
		if (0 == vScroll.y)
			goto NextCheck;

		_float fAngle = m_pTargetObject->GetRotationY();
		fAngle += vScroll.y * dt * 500.f;
		if (fAngle > 360.f)
			fAngle -= 360.f;
		if (fAngle < 0)
			fAngle += 360.f;
		m_pTargetObject->SetRotationY(fAngle);
	}

NextCheck:

	// Object Delete
	static _bool isDelDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_DELETE))
	{
		if (!isDelDown)
		{
			isDelDown = true;

			if (nullptr != m_pTargetObject)
			{
				m_pTargetObject->SetDead(true);
				m_pTargetObject->SetSelected(false);
				m_pTargetObject = nullptr;
			}
		}
	}
	else
		isDelDown = false;

	// Object Y position : Up
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_O))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y += dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	// Object Y position : Down
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_L))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y -= dt * 5.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}

	// Object Y position : Set 0
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_P))
	{
		if (nullptr != m_pTargetObject &&
			m_pDefaultCamera->GetMouseEnable())
		{
			vec3 vPos = m_pTargetObject->GetPosition();
			vPos.y = 0.f;
			m_pTargetObject->SetPosition(vPos);
		}
	}
}

void MapEditorUI::RenderSceneDetailUI()
{
	if (nullptr == m_pScene)
		return;

	Text(m_pString->GetString("TEXT_36").c_str());
	Text(m_pString->GetString("TEXT_37").c_str()); SameLine(120.f); Text(m_pScene->GetDataPath().c_str());
	Text(m_pString->GetString("TEXT_38").c_str()); SameLine(120.f); Text(m_pScene->GetObjListFileName().c_str());

	stringstream ss;
	ss << m_pString->GetString("TEXT_39") << "##Object List Save";
	if (Button(ss.str().c_str(), ImVec2(190.f, 0.f)))
	{
		ObjListSave();
	}
	SameLine(200.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_40") << "##Object List Load";
	if (Button(ss.str().c_str(), ImVec2(190.f, 0.f)))
	{
		ObjListLoad();
	}
}

void MapEditorUI::RenderObjectTargetDetailUI()
{
	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2, 0.2, 0.2, 1));
	if (BeginChild("##TargetSetting", ImVec2(380.f, 170.f)))
	{
		Text(m_pString->GetString("TEXT_41").c_str());
		if (nullptr == m_pTargetObject)
		{
			Text(m_pString->GetString("TEXT_42").c_str());
			PopStyleColor();
			EndChild();
			return;
		}

		Text(" ");
		SameLine(140.f);
		_bool isLock = m_pTargetObject->GetLock();
		stringstream ss;
		ss << m_pString->GetString("TEXT_43") << "##LockTarget";
		Checkbox(ss.str().c_str(), &isLock);
		m_pTargetObject->SetLock(isLock);

		SameLine(200.f);
		_bool isEnable = m_pTargetObject->GetEnable();
		ss.str("");
		ss << m_pString->GetString("TEXT_44") << "##ShowTarget";
		Checkbox(ss.str().c_str(), &isEnable);
		m_pTargetObject->SetEnable(isEnable);

		SameLine(260.f);
		_bool isDebug = m_pTargetObject->GetDebug();
		ss.str("");
		ss << m_pString->GetString("TEXT_45") << "##BoxTarget";
		Checkbox(ss.str().c_str(), &isDebug);
		m_pTargetObject->SetDebug(isDebug);

		SameLine(320.f);
		_bool isWire = m_pTargetObject->GetWireFrame();
		ss.str("");
		ss << m_pString->GetString("TEXT_46") << "##WireTarget";
		Checkbox(ss.str().c_str(), &isWire);
		m_pTargetObject->SetWireFrame(isWire);

		Text(m_pString->GetString("TEXT_47").c_str()); SameLine(80.f); Text(m_pTargetObject->GetMeshID().c_str());
		Text(" ");

		vec3 vPos = m_pTargetObject->GetPosition();
		ConvertFloatToCharArray(m_chPos[0], vPos.x);
		ConvertFloatToCharArray(m_chPos[1], vPos.y);
		ConvertFloatToCharArray(m_chPos[2], vPos.z);
		vec3 vRot = m_pTargetObject->GetRotation();
		ConvertFloatToCharArray(m_chRot[0], vRot.x);
		ConvertFloatToCharArray(m_chRot[1], vRot.y);
		ConvertFloatToCharArray(m_chRot[2], vRot.z);
		vec3 vScale = m_pTargetObject->GetScale();
		ConvertFloatToCharArray(m_chScale[0], vScale.x);
		ConvertFloatToCharArray(m_chScale[1], vScale.y);
		ConvertFloatToCharArray(m_chScale[2], vScale.z);
		string soundTag = m_pTargetObject->GetSoundTag();
		ss.str("");
		ss << soundTag;
		strcpy_s(m_chSound, ss.str().length() + 1, ss.str().c_str());

		Text(m_pString->GetString("TEXT_48").c_str());
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##PosX", m_chPos[0], sizeof(m_chPos[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##PosY", m_chPos[1], sizeof(m_chPos[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##PosZ", m_chPos[2], sizeof(m_chPos[2]));

		Text(m_pString->GetString("TEXT_49").c_str());
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##RotX", m_chRot[0], sizeof(m_chRot[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##RotY", m_chRot[1], sizeof(m_chRot[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##RotZ", m_chRot[2], sizeof(m_chRot[2]));

		Text(m_pString->GetString("TEXT_50").c_str());
		SameLine(80.f); Text("X:"); SameLine(100.f);
		SetNextItemWidth(70); InputText("##ScaleX", m_chScale[0], sizeof(m_chScale[0]));
		SameLine(180.f); Text("Y:"); SameLine(200.f);
		SetNextItemWidth(70); InputText("##ScaleY", m_chScale[1], sizeof(m_chScale[1]));
		SameLine(280.f); Text("Z:"); SameLine(300.f);
		SetNextItemWidth(70); InputText("##ScaleZ", m_chScale[2], sizeof(m_chScale[2]));

		Text(m_pString->GetString("TEXT_79").c_str());
		SameLine(80.f); SetNextItemWidth(210); InputText("##SoundTag", m_chSound, sizeof(m_chSound));

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
		m_pTargetObject->SetPosition(vec3(fPosX, fPosY, fPosZ));
		m_pTargetObject->SetRotation(vec3(fRotX, fRotY, fRotZ));
		m_pTargetObject->SetScale(vec3(fScaleX, fScaleY, fScaleZ));
		m_pTargetObject->SetSoundTag(m_chSound);
	}
	PopStyleColor();
	EndChild();
}

void MapEditorUI::RenderObjectList(_float screenX, _float screenY)
{
	if (nullptr == m_pBGLayer)
		return;

	Text(m_pString->GetString("TEXT_51").c_str());
	Text(" ");
	SameLine(148.f);
	stringstream ss;
	ss << m_pString->GetString("TEXT_43") << "##LockZero";
	Checkbox(ss.str().c_str(), &m_isZeroLock);
	if (m_isPreviousZeroLock != m_isZeroLock)
	{
		m_isPreviousZeroLock = m_isZeroLock;
		list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			(*iter)->SetLock(m_isZeroLock);
	}

	SameLine(208.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_44") << "##ShowZero";
	Checkbox(ss.str().c_str(), &m_isZeroShow);
	if (m_isPreviousZeroShow != m_isZeroShow)
	{
		m_isPreviousZeroShow = m_isZeroShow;
		list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			(*iter)->SetEnable(m_isZeroShow);
	}

	SameLine(268.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_45") << "##BoxZero";
	Checkbox(ss.str().c_str(), &m_isZeroDebug);
	if (m_isPreviousZeroDebug != m_isZeroDebug)
	{
		m_isPreviousZeroDebug = m_isZeroDebug;
		list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			(*iter)->SetDebug(m_isZeroDebug);
	}

	SameLine(318.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_46") << "##WireZero";
	Checkbox(ss.str().c_str(), &m_isZeroWire);
	if (m_isPreviousZeroWire != m_isZeroWire)
	{
		m_isPreviousZeroWire = m_isZeroWire;
		list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
			(*iter)->SetWireFrame(m_isZeroWire);
	}

	if (BeginChild("##ObjectLists", ImVec2(390.f, screenY - 350.f)))
	{
		_uint index = 0;
		list<CGameObject*>* listObj = m_pBGLayer->GetObjectList();
		list<CGameObject*>::iterator iter;
		for (iter = listObj->begin(); iter != listObj->end(); ++iter)
		{
			ss.str("");
			ss << (*iter)->GetName() << "##Pick_" << index;
			if (Button(ss.str().c_str(), ImVec2(120.f, 0.f)))
			{
				if (nullptr != m_pTargetObject)
					m_pTargetObject->SetSelected(false);
				m_pTargetObject = dynamic_cast<BGObject*>(*iter);
				m_pTargetObject->SetSelected(true);
			}

			SameLine(140.f);
			_bool isLock = (*iter)->GetLock();
			ss.str("");  ss << m_pString->GetString("TEXT_43") << "##Lock" << index;
			Checkbox(ss.str().c_str(), &isLock);
			(*iter)->SetLock(isLock);

			SameLine(200.f);
			_bool isEnable = (*iter)->GetEnable();
			ss.str("");  ss << m_pString->GetString("TEXT_44") << "##Show" << index;
			Checkbox(ss.str().c_str(), &isEnable);
			(*iter)->SetEnable(isEnable);

			SameLine(260.f);
			_bool isDebug = (*iter)->GetDebug();
			ss.str("");  ss << m_pString->GetString("TEXT_45") << "##Box" << index;
			Checkbox(ss.str().c_str(), &isDebug);
			(*iter)->SetDebug(isDebug);

			SameLine(310.f);
			_bool isWire = (*iter)->GetWireFrame();
			ss.str("");  ss << m_pString->GetString("TEXT_46") << "##Wire" << index;
			Checkbox(ss.str().c_str(), &isWire);
			(*iter)->SetWireFrame(isWire);

			++index;
		}
	}
	EndChild();
}

void MapEditorUI::RenderLightDetailUI()
{
	if (nullptr == m_pScene)
		return;

	Text(m_pString->GetString("TEXT_52").c_str());
	Text(m_pString->GetString("TEXT_53").c_str()); SameLine(120.f); Text(m_pScene->GetDataPath().c_str());
	Text(m_pString->GetString("TEXT_54").c_str()); SameLine(120.f); Text(m_pScene->GetLightListFileName().c_str());

	stringstream ss;
	ss << m_pString->GetString("TEXT_55") << "##Light List Save";
	if (Button(ss.str().c_str(), ImVec2(190.f, 0.f)))
	{
		LightListSave();
	}
	SameLine(200.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_56") << "##Light List Load";
	if (Button(ss.str().c_str(), ImVec2(190.f, 0.f)))
	{
		LightListLoad();
	}
	Text(m_pString->GetString("TEXT_57").c_str());

	ss.str("");
	ss << m_pString->GetString("TEXT_58") << "##DirectionalLightAdd";
	if (Button(ss.str().c_str(), ImVec2(125.f, 0.f)))
	{
		CLight::cLightInfo* pInfo = new CLight::cLightInfo();
		pInfo->name = "Directional Light";
		pInfo->param1.x = 0;
		pInfo->param2.x = 1;
		CLightMaster::GetInstance()->AddLight(pInfo);
	}
	SameLine(135.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_59") << "##PointLightAdd";
	if (Button(ss.str().c_str(), ImVec2(128.f, 0.f)))
	{
		CLight::cLightInfo* pInfo = new CLight::cLightInfo();
		pInfo->name = "Point Light";
		pInfo->param1.x = 1;
		pInfo->param2.x = 1;
		CLightMaster::GetInstance()->AddLight(pInfo);
	}
	SameLine(265.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_60") << "##SpotLightAdd";
	if (Button(ss.str().c_str(), ImVec2(125.f, 0.f)))
	{
		CLight::cLightInfo* pInfo = new CLight::cLightInfo();
		pInfo->name = "Spot Light";
		pInfo->param1.x = 2;
		pInfo->param2.x = 1;
		CLightMaster::GetInstance()->AddLight(pInfo);
	}
	ss.str("");
	ss << m_pString->GetString("TEXT_61") << "##Copy Current Light##LightAdd";
	if (Button(ss.str().c_str(), ImVec2(382.f, 0.f)))
	{
		if (nullptr != m_pTargetLight)
		{
			CLight::cLightInfo* pInfo = new CLight::cLightInfo();
			CLight::cLightInfo* srcInfo = m_pTargetLight->GetLightInfo();

			stringstream ss;
			ss << srcInfo->name << " (Copy)";
			pInfo->name = ss.str();
			pInfo->position = srcInfo->position;
			pInfo->direction = srcInfo->direction;
			pInfo->diffuse = srcInfo->diffuse;
			pInfo->specular = srcInfo->specular;
			pInfo->ambient = srcInfo->ambient;
			pInfo->atten = srcInfo->atten;
			pInfo->param1 = srcInfo->param1;
			pInfo->param2 = srcInfo->param2;

			CLightMaster::GetInstance()->AddLight(pInfo);
		}
	}
}

void MapEditorUI::RenderLightTargetDetailUI()
{
	PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2, 0.2, 0.2, 1));
	if (BeginChild("##LightSetting", ImVec2(380.f, 225.f)))
	{
		Text(m_pString->GetString("TEXT_62").c_str());
		if (nullptr == m_pTargetLight)
		{
			Text(m_pString->GetString("TEXT_63").c_str());
			Text(" ");
		}
		else
		{
			CLight::cLightInfo* pInfo = m_pTargetLight->GetLightInfo();
			strcpy_s(m_chLightName, pInfo->name.c_str());
			Text(m_pString->GetString("TEXT_64").c_str()); SameLine(88.f); SetNextItemWidth(300); InputText("##LightName", m_chLightName, sizeof(m_chLightName));
			pInfo->name = m_chLightName;

			// position
			vec4 value = pInfo->position;
			ConvertFloatToCharArray(m_chLightPos[0], value.x);
			ConvertFloatToCharArray(m_chLightPos[1], value.y);
			ConvertFloatToCharArray(m_chLightPos[2], value.z);
			ConvertFloatToCharArray(m_chLightPos[3], value.w);

			Text(m_pString->GetString("TEXT_48").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightPosX", m_chLightPos[0], sizeof(m_chLightPos[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightPosY", m_chLightPos[1], sizeof(m_chLightPos[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightPosZ", m_chLightPos[2], sizeof(m_chLightPos[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightPosW", m_chLightPos[3], sizeof(m_chLightPos[3]));

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

			Text(m_pString->GetString("TEXT_65").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightDirX", m_chLightDir[0], sizeof(m_chLightDir[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightDirY", m_chLightDir[1], sizeof(m_chLightDir[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightDirZ", m_chLightDir[2], sizeof(m_chLightDir[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightDirW", m_chLightDir[3], sizeof(m_chLightDir[3]));

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

			Text(m_pString->GetString("TEXT_66").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightDiffX", m_chLightDiff[0], sizeof(m_chLightDiff[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightDiffY", m_chLightDiff[1], sizeof(m_chLightDiff[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightDiffZ", m_chLightDiff[2], sizeof(m_chLightDiff[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightDiffW", m_chLightDiff[3], sizeof(m_chLightDiff[3]));

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

			Text(m_pString->GetString("TEXT_67").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightSpecX", m_chLightspec[0], sizeof(m_chLightspec[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightSpecY", m_chLightspec[1], sizeof(m_chLightspec[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightSpecZ", m_chLightspec[2], sizeof(m_chLightspec[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightSpecW", m_chLightspec[3], sizeof(m_chLightspec[3]));

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

			Text(m_pString->GetString("TEXT_68").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightAmbiX", m_chLightambi[0], sizeof(m_chLightambi[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightAmbiY", m_chLightambi[1], sizeof(m_chLightambi[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightAmbiZ", m_chLightambi[2], sizeof(m_chLightambi[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightAmbiW", m_chLightambi[3], sizeof(m_chLightambi[3]));

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

			Text(m_pString->GetString("TEXT_69").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##LightAttenX", m_chLightatten[0], sizeof(m_chLightatten[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##LightAttenY", m_chLightatten[1], sizeof(m_chLightatten[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##LightAttenZ", m_chLightatten[2], sizeof(m_chLightatten[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##LightAttenW", m_chLightatten[3], sizeof(m_chLightatten[3]));

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

			Text(m_pString->GetString("TEXT_70").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##Lightparam1X", m_chLightparam1[0], sizeof(m_chLightparam1[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##Lightparam1Y", m_chLightparam1[1], sizeof(m_chLightparam1[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##Lightparam1Z", m_chLightparam1[2], sizeof(m_chLightparam1[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##Lightparam1W", m_chLightparam1[3], sizeof(m_chLightparam1[3]));

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

			Text(m_pString->GetString("TEXT_71").c_str());
			SameLine(70.f); Text("X:"); SameLine(88.f); SetNextItemWidth(50); InputText("##Lightparam2X", m_chLightparam2[0], sizeof(m_chLightparam2[0]));
			SameLine(148.f); Text("Y:"); SameLine(166.f); SetNextItemWidth(50); InputText("##Lightparam2Y", m_chLightparam2[1], sizeof(m_chLightparam2[1]));
			SameLine(226.f); Text("Z:"); SameLine(244.f); SetNextItemWidth(50); InputText("##Lightparam2Z", m_chLightparam2[2], sizeof(m_chLightparam2[2]));
			SameLine(304.f); Text("W:"); SameLine(322.f); SetNextItemWidth(50); InputText("##Lightparam2W", m_chLightparam2[3], sizeof(m_chLightparam2[3]));

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
}

void MapEditorUI::RenderLightList()
{
	Text(m_pString->GetString("TEXT_72").c_str());
	vector<CLight*>* pLightVector = CLightMaster::GetInstance()->GetLightVector();
	for (int i = 0; i < pLightVector->size(); ++i)
	{
		CLight* pLight = (*pLightVector)[i];

		stringstream ss;
		ss << pLight->GetLightInfo()->name << "##PickLight_" << i;
		if (Button(ss.str().c_str(), ImVec2(240.f, 0.f)))
		{
			m_pTargetLight = pLight;
		}
		SameLine(250.f);
		ss.str("");
		ss << m_pString->GetString("TEXT_73") << "##DeleteLight_" << i;
		if (Button(ss.str().c_str(), ImVec2(140.f, 0.f)))
		{
			if (m_pTargetLight == pLight)
				m_pTargetLight = nullptr;
			CLightMaster::GetInstance()->RemoveLight(pLight);
		}
	}
}

void MapEditorUI::RenderMeshList()
{
	Text(" ");
	SameLine(110.f); Text(m_pString->GetString("TEXT_74").c_str());
	SameLine(300.f); Text(m_pString->GetString("TEXT_75").c_str());

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
					, vPos, vRot, vScale, "");

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

void MapEditorUI::ConvertFloatToCharArray(char* dest, _float value)
{
	stringstream ss;
	ss << value;
	strcpy_s(dest, ss.str().length() + 1, ss.str().c_str());
}

void MapEditorUI::ObjListSave()
{
	if (nullptr != m_pScene)
		m_pScene->SaveBackgroundObjects();
}

void MapEditorUI::ObjListLoad()
{
	if (nullptr != m_pScene)
		m_pScene->LoadBackgroundObjects();

	if (nullptr != m_pTargetObject)
	{
		m_pTargetObject->SetSelected(false);
		m_pTargetObject = nullptr;
	}

	m_isPreviousZeroLock = false;
	m_isZeroLock = false;
	m_isPreviousZeroShow = true;
	m_isZeroShow = true;
	m_isPreviousZeroDebug = true;
	m_isZeroDebug = false;
	m_isPreviousZeroWire = false;
	m_isZeroWire = false;
}

void MapEditorUI::LightListSave()
{
	if (nullptr == m_pScene)
		return;

	CLightMaster::GetInstance()->SaveLights(m_pScene->GetDataPath(), m_pScene->GetLightListFileName());
}

void MapEditorUI::LightListLoad()
{
	if (nullptr == m_pScene)
		return;

	m_pTargetLight = nullptr;

	CLightMaster::GetInstance()->LoadLights(m_pScene->GetDataPath(), m_pScene->GetLightListFileName());
}

MapEditorUI* MapEditorUI::Create(CScene* pScene)
{
	MapEditorUI* pInstance = new MapEditorUI();
	if (PK_NOERROR != pInstance->Ready(pScene))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

