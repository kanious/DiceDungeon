#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CScene;
}

class MapEditorUIManager;
class SoundUIManager;

class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)

private:
	MapEditorUIManager*		m_pMapEditorUI;
	SoundUIManager*			m_pSoundUI;
	Engine::CScene*			m_pCurrentScene;

private:
	explicit UIManager();
	~UIManager();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene);

public:
	_bool GetCursorIsOnTheUI();
};

#endif //_UIMANAGER_H_