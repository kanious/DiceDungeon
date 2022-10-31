#ifndef _MAPEDITORUIMANAGER_H_
#define _MAPEDITORUIMANAGER_H_

#include "EngineDefines.h"

namespace Engine
{
	class CScene;
	class CLayer;
	class CLight;
}

class BGObject;

class MapEditorUIManager
{
	SINGLETON(MapEditorUIManager)
private:
	class MeshInfo
	{
	public:
		std::string meshId;
		char* inputSize;
	};

private:
	std::vector<MeshInfo>			m_vecMeshInfo;
	Engine::CScene*					m_pScene;
	Engine::CLayer**				m_ppBGLayer;
	BGObject**						m_ppTargetObject;

	//For.Target_Obj_Detail_Setting
	char							m_chPos[3][128];
	char							m_chRot[3][128];
	char							m_chScale[3][128];

	//For.Target_Light_Setting
	Engine::CLight*					m_pTargetLight;
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

public:
	_bool GetCursorIsOnTheUI();

private:
	explicit MapEditorUIManager();
	~MapEditorUIManager();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene, BGObject** ppTarget, Engine::CLayer** ppLayer);

private:
	void RenderSceneDetailUI();
	void RenderTargetDetailUI();
	void RenderObjectList();
	void RenderLightList();
private:
	void RenderMeshList();
private:
	void ConvertFloatToCharArray(char* src, _float value);
	void ObjListSave();
	void ObjListLoad();
};

#endif //_MAPEDITORUIMANAGER_H_