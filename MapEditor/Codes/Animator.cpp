#include "Animator.h"
#include "Animation.h"
#include "AnimationData.h"
#include "Player.h"
#include "Function.h"
#include "LuaBrain.h"


USING(Engine)
USING(glm)
USING(std)

Animator::Animator()
    : m_pPlayer(nullptr), m_currentTag(""), m_bBlendingOption(true)
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
void Animator::ChangeAnimation(string tag)
{
    if (m_currentTag == tag)
        return;

    if (m_bBlendingOption)
        StartBlending();
    m_currentTag = tag;
    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation(tag);
    ResetAnimation();
}

void Animator::FrameMove(const _float& dt)
{
    CAnimController::FrameMove(dt);

    if ("true_pumpkin_king_walk" == m_currentTag)
    {
        if (2 == m_iFrameIndex || 17 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Walk");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_run" == m_currentTag)
    {
        if (2 == m_iFrameIndex || 10 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Run");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_attack1" == m_currentTag)
    {
        if (5 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Attack1");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_attack2" == m_currentTag)
    {
        if (5 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Attack2");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_grab" == m_currentTag)
    {
        if (5 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Grab");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_pick_up" == m_currentTag)
    {
        if (5 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Pickup");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_hurt" == m_currentTag)
    {
        if (4 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Hurt02");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
    else if ("true_pumpkin_king_death" == m_currentTag)
    {
        if (5 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                CLuaBrain::GetInstance()->RunLuaScript("true_pumpkin_king_Death");
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }
}

// Initialize
RESULT Animator::Ready(Player* pPlayer)
{
    m_pPlayer = pPlayer;

    m_fAnimSpeed = 1.f;

    m_currentTag = "true_pumpkin_king_idle";
    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation("true_pumpkin_king_idle");
    m_pvecPrevTransforms = GetMatrix();

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
