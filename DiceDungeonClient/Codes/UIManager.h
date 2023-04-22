#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class iPhysicsWorld;
}
class SceneDungeon;
class Player;
class UIHealthInfo;

// Manages all UI
class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)
private:
	Engine::iPhysicsWorld*		m_pPWorld;
	std::string					m_Message;
	SceneDungeon*				m_pScene;
	Player*						m_pPlayer;

	char						m_char[3][128];
	_int						m_iHighScore;

	_bool						m_bOpenRankingListPage;

	_bool						m_bOpenPlayerMoveW;
	glm::vec3					m_vecMoveTilePos;

	std::vector<UIHealthInfo*>	m_vecHPInfos;

	_bool						m_bDeferredTexture;
	_bool						m_bHPUI;
	_bool						m_bBGPlay;
	_bool						m_bBGPlayPrev;
	_bool						m_bAnimBlending;

private:
	explicit UIManager();
	virtual ~UIManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Basic Render Function
	void RenderUI();
	// Initialize
	RESULT Ready(Engine::iPhysicsWorld* pWorld, SceneDungeon* pScene);

private:
	void RenderDiceWindow();
	void RenderScoreWindow(_float width, _float height);
	void RenderDebugUI(_float width, _float height);

public:
	void SetPlayerMoveButtonWindow(_bool bOpen, glm::vec3 vPos = glm::vec3(0.f));
	void AddHealthUI(UIHealthInfo* pUI);
	_bool GetCursorIsOnTheUI();

private:
	// Top 20 ranking list
	void RenderRankingPage(_float width, _float height);
	void RenderPlayerMoveButtonWindow(_float width, _float height);
};

#endif //_UIMANAGER_H_