#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

namespace Engine
{
	class iPhysicsWorld;
}

// Manages all UI
class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)
private:
	Engine::iPhysicsWorld*		m_pPWorld;
	std::string					m_Message;

	char						m_char[3][128];
	_int						m_iHighScore;

	_bool						m_bOpenRankingListPage;

private:
	explicit UIManager();
	~UIManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Basic Render Function
	void RenderUI();
	// Initialize
	RESULT Ready(Engine::iPhysicsWorld* pWorld);

private:
	// Top 20 ranking list
	void RenderRankingPage(_float width, _float height);
};

#endif //_UIMANAGER_H_