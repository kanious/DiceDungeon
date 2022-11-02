#ifndef _MAPEDITORUI_H_
#define _MAPEDITORUI_H_

#include "Base.h"

namespace Engine
{
	class CScene;
	class CLayer;
	class CLight;
	class CInputDevice;
}

class UIManager;
class DefaultCamera;
class BGObject;
class PumpkinString;

class MapEditorUI : public Engine::CBase
{
private:
	class MeshInfo
	{
	public:
		std::string meshId;
		char* inputSize;
	};

private:
	UIManager*						m_pUIManager;
	std::vector<MeshInfo>			m_vecMeshInfo;
	Engine::CScene*					m_pScene;
	Engine::CLayer*					m_pBGLayer;
	DefaultCamera*					m_pDefaultCamera;
	BGObject*						m_pTargetObject;
	Engine::CInputDevice*			m_pInputDevice;
	PumpkinString*					m_pString;

	//For.Target_Obj_Detail_Setting
	char							m_chPos[3][128];
	char							m_chRot[3][128];
	char							m_chScale[3][128];
	char							m_chSound[128];

	//For.Target_Light_Setting
	Engine::CLight*					m_pTargetLight;
	char							m_chLightName[128];
	char							m_chLightPos[4][128];
	char							m_chLightDir[4][128];
	char							m_chLightDiff[4][128];
	char							m_chLightspec[4][128];
	char							m_chLightambi[4][128];
	char							m_chLightatten[4][128];
	char							m_chLightparam1[4][128];
	char							m_chLightparam2[4][128];

	//For.AllCheck
	_bool							m_isPreviousZeroLock;
	_bool							m_isZeroLock;
	_bool							m_isPreviousZeroShow;
	_bool							m_isZeroShow;
	_bool							m_isPreviousZeroDebug;
	_bool							m_isZeroDebug;
	_bool							m_isPreviousZeroWire;
	_bool							m_isZeroWire;

private:
	explicit MapEditorUI();
	~MapEditorUI();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene);

public:
	void KeyCheck(const _float& dt);

private:
	//Objects
	void RenderSceneDetailUI();
	void RenderObjectTargetDetailUI();
	void RenderObjectList(_float screenX, _float screenY);
	//Lights
	void RenderLightDetailUI();
	void RenderLightTargetDetailUI();
	void RenderLightList();
private:
	void RenderMeshList();
private:
	void ConvertFloatToCharArray(char* src, _float value);
	void ObjListSave();
	void ObjListLoad();
	void LightListSave();
	void LightListLoad();

public:
	static MapEditorUI* Create(Engine::CScene* pScene);
};

#endif //_MAPEDITORUI_H_