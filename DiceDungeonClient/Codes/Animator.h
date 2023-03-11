#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "AnimController.h"
#include "Enums.h"

class Player;

// Child class of Animation Controller to manage animations per object
class Animator : public Engine::CAnimController
{
private:
	Player*		m_pPlayer;


private:
	explicit Animator();
	virtual ~Animator();

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Call this function when animation has ended
	virtual void AnimationEndEvent();
	// Set animation by type state
	void SetAnimationByType();

private:
	// Initialize
	RESULT Ready(Player* pPlayer);
public:
	// Create an instance
	static Animator* Create(Player* pPlayer);
};

#endif //_ANIMATOR_H_