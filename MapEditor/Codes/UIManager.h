#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

class SystemUI;
class MapEditorUI;
class Scene3D;

class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)

public:
	enum eUIType
	{
		UI_MAP_EDITOR_WINDOW,
		UI_END
	};

private:
	_bool				m_bUIOpen[UI_END];

	MapEditorUI*		m_pMapEditorUI;

	Scene3D*			m_pScene;

private:
	explicit UIManager();
	~UIManager();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();

public:
	void SetImGuiStyle();
	_bool GetCursorIsOnTheUI();
	_bool GetUIOpened(eUIType type)		{ return m_bUIOpen[type]; }
	void SetUIToggle(eUIType type)		{ m_bUIOpen[type] = !m_bUIOpen[type]; }
	void SetAllUIClose();

public:
	RESULT Ready(Scene3D* pScene);
};

#endif //_UIMANAGER_H_