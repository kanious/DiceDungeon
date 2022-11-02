#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CScene;
}

class SystemUI;
class MapEditorUI;
class SoundUI;

class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)

public:
	enum eUIType
	{
		UI_SYSTEM_MENU_WINDOW,
		UI_MAP_EDITOR_WINDOW,
		UI_SOUND_EDITOR_WINDOW,
		UI_END
	};

private:
	_bool					m_bUIOpen[UI_END];

	SystemUI*				m_pSystemUI;
	MapEditorUI*			m_pMapEditorUI;
	SoundUI*				m_pSoundUI;

private:
	explicit UIManager();
	~UIManager();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene);

public:
	_bool GetCursorIsOnTheUI();
	_bool GetUIOpened(eUIType type)				{ return m_bUIOpen[type]; }
	_bool GetAnyUIOpened();
	void SetUI(eUIType type);
	void SetAllUIClose();

};

#endif //_UIMANAGER_H_