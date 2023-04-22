#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "AnimController.h"
#include "Enums.h"

namespace Engine
{
	class CGameObject;
}
class SceneDungeon;
class Player;
class Enemy;

// Child class of Animation Controller to manage animations per object
class Animator : public Engine::CAnimController
{
private:
	std::string		m_currentTag;
	_bool			m_bBlendingOption;
	SceneDungeon*	m_pScene;
	Player*			m_pPlayer;
	Enemy*			m_pEnemy;

private:
	explicit Animator();
	virtual ~Animator();

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Call this function when animation has ended
	virtual void AnimationEndEvent();

public:
	void SetBlendingOption(_bool value) { m_bBlendingOption = value; }
	_bool GetBlendingOption() { return m_bBlendingOption; }
	// Set animation by tag
	void ChangeAnimation(std::string tag);
	virtual void FrameMove(const _float& dt);

private:
	// Initialize
	RESULT Ready(std::string tag, _bool isEnemy, Engine::CGameObject* pObj, SceneDungeon* pScene);
public:
	// Create an instance
	static Animator* Create(std::string tag, _bool isEnemy, Engine::CGameObject* pObj, SceneDungeon* pScene);
};

#endif //_ANIMATOR_H_