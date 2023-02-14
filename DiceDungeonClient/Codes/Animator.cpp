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

void Animator::Destroy()
{
    SafeDestroy(m_pCurAnimation);
}

void Animator::AnimationEndEvent()
{
}

void Animator::SetAnimationByType()
{
    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation("jump");
}

RESULT Animator::Ready(Player* pPlayer)
{
    m_pPlayer = pPlayer;

    m_fAnimSpeed = 3.f;

    SetAnimationByType();

	return PK_NOERROR;
}

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
