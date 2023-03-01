#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "AnimController.h"
#include "Enums.h"

class Player;

class Animator : public Engine::CAnimController
{
private:
	Player*			m_pPlayer;


private:
	explicit Animator();
	virtual ~Animator();

private:
	virtual void Destroy();
	virtual void AnimationEndEvent();
	void ChangeAnimationByType();

private:
	RESULT Ready(Player* pPlayer);
public:
	static Animator* Create(Player* pPlayer);
};

#endif //_ANIMATOR_H_