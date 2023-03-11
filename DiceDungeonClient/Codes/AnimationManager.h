#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CInputDevice;
}
class Player;
class Animator;

// A class that oversees the client's animator
class AnimationManager : public Engine::CBase
{
	SINGLETON(AnimationManager)

private:
	Engine::CInputDevice*			m_pInputDevice;
	std::vector<Animator*>			m_vecAnimators;

private:
	explicit AnimationManager();
	~AnimationManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready();

public:
	// Basic Update Function, update all animators
	void Update(const _float& dt);

public:
	// Add Animator
	void AddAnimator(Animator* pAnimator);
};

#endif //_ANIMATIONMANAGER_H_