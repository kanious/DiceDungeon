#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

class SystemUI;
class MapEditorUI;
class Scene3D;

// Manages all UI
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
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Update all UIs
	void Update(const _float& dt);
	// Basic Render Function
	void RenderUI();

public:
	// Set ImGui Style
	void SetImGuiStyle();
	// Check if mouse cursor is in the UI area
	_bool GetCursorIsOnTheUI();
	_bool GetUIOpened(eUIType type)		{ return m_bUIOpen[type]; }
	void SetUIToggle(eUIType type)		{ m_bUIOpen[type] = !m_bUIOpen[type]; }
	// Close all UI
	void SetAllUIClose();

public:
	// Initialize
	RESULT Ready(Scene3D* pScene);
};

#endif //_UIMANAGER_H_