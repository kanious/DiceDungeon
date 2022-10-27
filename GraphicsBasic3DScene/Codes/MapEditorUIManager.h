#ifndef _MAPEDITORUIMANAGER_H_
#define _MAPEDITORUIMANAGER_H_

#include "EngineDefines.h"

namespace Engine
{
	class CScene;
}

class MapEditorUIManager
{
	SINGLETON(MapEditorUIManager)

private:
	std::vector<std::string>		m_vecMeshList;
	std::vector<std::string>		m_vecTextList;
	Engine::CScene*					m_pScene;

private:
	explicit MapEditorUIManager();
	~MapEditorUIManager();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene);

private:
	void RenderMeshList();
};

#endif //_MAPEDITORUIMANAGER_H_