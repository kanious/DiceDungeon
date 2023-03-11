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
};

#endif //_UIMANAGER_H_