#ifndef _TARGETMANAGER_H_
#define _TARGETMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}

class Player;
class SceneDungeon;

// A class that searches for and manages the target object to be moved by the user
class TargetManager : public Engine::CBase
{
	SINGLETON(TargetManager)

private:
	Engine::CInputDevice*		m_pInputDevice;
	Player*						m_pTarget;
	SceneDungeon*				m_pScene;
	Engine::CLayer*				m_pCharacterLayer;

private:
	explicit TargetManager();
	virtual ~TargetManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(SceneDungeon* pScene);

public:
	// Basic Update Function, check user input
	void Update(const _float& dt);

private:
	// User Input Check
	void KeyCheck(const _float& dt);
	// Check if the user's mouse click has selected a character
	_bool FindTargetAndSet();
	// Move the target
	void MoveTarget(_bool bSnap);
	// Release the target
	void ReleaseTarget();
};

#endif //_TARGETMANAGER_H_