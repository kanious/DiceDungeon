#include "Animator.h"
#include "Animation.h"
#include "AnimationData.h"
#include "Player.h"
#include "Function.h"


USING(Engine)
USING(glm)
USING(std)

Animator::Animator()
    : m_pPlayer(nullptr), m_bIsRandom(false), m_eAnimType(ANIM_END), m_eEaseType(EASE_END)
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
    if (m_bIsRandom)
    {
        int rand = GetRandNum(0, 3);
        m_eEaseType = (eEaseType)rand;
        SetAnimationByType();
        if (nullptr != m_pPlayer)
            m_pPlayer->SetEaseType(m_eEaseType);
    }
}

void Animator::SetAnimationByType()
{
    string tag = "";
    switch (m_eEaseType)
    {
    case EASE_NONE:

        if (ANIM_MOVING == m_eAnimType)
            tag = "moving";
        else if (ANIM_ROTATING == m_eAnimType)
            tag = "rotating";
        else
            tag = "scaling";

        break;

    case EASE_IN:

        if (ANIM_MOVING == m_eAnimType)
            tag = "moving_easein";
        else if (ANIM_ROTATING == m_eAnimType)
            tag = "rotating_easein";
        else
            tag = "scaling_easein";

        break;

    case EASE_OUT:
        
        if (ANIM_MOVING == m_eAnimType)
            tag = "moving_easeout";
        else if (ANIM_ROTATING == m_eAnimType)
            tag = "rotating_easeout";
        else
            tag = "scaling_easeout";

        break;

    case EASE_INOUT:
        
        if (ANIM_MOVING == m_eAnimType)
            tag = "moving_easeinout";
        else if (ANIM_ROTATING == m_eAnimType)
            tag = "rotating_easeinout";
        else
            tag = "scaling_easeinout";

        break;
    }

    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation(tag);
}

RESULT Animator::Ready(eAnimType animType, eEaseType easeType, Player* pPlayer, _bool isRandom)
{
    m_eAnimType = animType;
    m_eEaseType = easeType;
    m_pPlayer = pPlayer;
    m_bIsRandom = isRandom;

    m_fAnimSpeed = 3.f;

    SetAnimationByType();

	return PK_NOERROR;
}

Animator* Animator::Create(eAnimType animType, eEaseType easeType, Player* pPlayer, _bool isRandom)
{
    Animator* pInstance = new Animator();
    if (PK_NOERROR != pInstance->Ready(animType, easeType, pPlayer, isRandom))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
