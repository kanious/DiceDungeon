#include "Animator.h"
#include "Animation.h"
#include "AnimationData.h"
#include "Player.h"
#include "Function.h"


USING(Engine)
USING(glm)
USING(std)

Animator::Animator()
    : m_pPlayer(nullptr)
{
}

Animator::~Animator()
{
}

// Call instead of destructor to manage class internal data
void Animator::Destroy()
{
    CAnimController::Destroy();
}

// Call this function when animation has ended
void Animator::AnimationEndEvent()
{
}

// Set animation by type state
void Animator::SetAnimationByType()
{
    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation("jump");
}

// Initialize
RESULT Animator::Ready(Player* pPlayer)
{
    m_pPlayer = pPlayer;

    m_fAnimSpeed = 3.f;

    SetAnimationByType();

	return PK_NOERROR;
}

// Create an instance
Animator* Animator::Create(Player* pPlayer)
{
    Animator* pInstance = new Animator();
    if (PK_NOERROR != pInstance->Ready(pPlayer))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
