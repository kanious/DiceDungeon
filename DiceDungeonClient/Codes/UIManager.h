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
	void Destroy();
	void RenderUI();
	RESULT Ready(Engine::iPhysicsWorld* pWorld);
};

#endif //_UIMANAGER_H_