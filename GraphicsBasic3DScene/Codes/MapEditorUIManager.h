#ifndef _MAPEDITORUIMANAGER_H_
#define _MAPEDITORUIMANAGER_H_

#include "EngineDefines.h"

namespace Engine
{
	class CScene;
}

class BGObject;

class MapEditorUIManager
{
	SINGLETON(MapEditorUIManager)

private:
	std::vector<std::string>		m_vecMeshList;
	std::vector<char*>				m_vecInputList;
	Engine::CScene*					m_pScene;
	BGObject**						m_ppTargetObject;

	//For.Target_Detail_Setting
	char							m_chPosX[128];
	char							m_chPosY[128];
	char							m_chPosZ[128];
	char							m_chRotX[128];
	char							m_chRotY[128];
	char							m_chRotZ[128];
	char							m_chScaleX[128];
	char							m_chScaleY[128];
	char							m_chScaleZ[128];

public:
	_bool GetCursorIsOnTheUI();

private:
	explicit MapEditorUIManager();
	~MapEditorUIManager();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene, BGObject** ppTarget);

private:
	void RenderTargetDetailUI(_float screenX, _float screenY);
	void RenderMeshList(_float screenX, _float screenY);
private:
	void ConvertFloatToCharArray(char* src, _float value);
};

#endif //_MAPEDITORUIMANAGER_H_