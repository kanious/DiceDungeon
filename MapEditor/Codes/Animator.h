#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "AnimController.h"
#include "Enums.h"

class Player;

class Animator : public Engine::CAnimController
{
private:
	Player*			m_pPlayer;
	_bool			m_bIsRandom;

	eAnimType		m_eAnimType;
	eEaseType		m_eEaseType;


private:
	explicit Animator();
	virtual ~Animator();

private:
	virtual void Destroy();
	virtual void AnimationEndEvent();
	void SetAnimationByType();

private:
	RESULT Ready(eAnimType animType, eEaseType easeType, Player* pPlayer, _bool isRandom);
public:
	static Animator* Create(eAnimType animType, eEaseType easeType, Player* pPlayer, _bool isRandom);
};

#endif //_ANIMATOR_H_