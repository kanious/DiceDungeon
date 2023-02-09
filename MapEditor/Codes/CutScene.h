#ifndef _CUTSCENE_H_
#define _CUTSCENE_H_

#include "Base.h"

class Animator;

class CutScene : public Engine::CBase
{
	SINGLETON(CutScene)

private:
	std::vector<Animator*> m_vecAnimators;


private:
	explicit CutScene();
	~CutScene();
public:
	void Destroy();
	RESULT Ready();

public:
	void AddAnimator(Animator* pAnimator);
};

#endif //_CUTSCENE_H_