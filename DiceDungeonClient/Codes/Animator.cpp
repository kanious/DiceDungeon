#include "Animator.h"
#include "Animation.h"
#include "AnimationData.h"
#include "GameObject.h"
#include "SceneDungeon.h"
#include "Player.h"
#include "Enemy.h"
#include "Function.h"
#include "LuaBrain.h"


USING(Engine)
USING(glm)
USING(std)

Animator::Animator()
    : m_currentTag(""), m_bBlendingOption(true), m_pPlayer(nullptr), m_pEnemy(nullptr)
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
    if ("true_dummy_attack1" == m_currentTag ||
        "true_crawler_attack" == m_currentTag)
    {
        m_pEnemy->SetMotionEnd(true);
    }
    else if ("true_pumpkin_king_hurt" == m_currentTag)
        m_pPlayer->SetMotionEnd(true);
    else if ("true_pumpkin_king_death" == m_currentTag)
    {
        SetPreviousFrame();
        m_bPause = true;
    }
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

    if ("true_dummy_attack1" == m_currentTag ||
        "true_crawler_attack" == m_currentTag)
    {
        if (14 == m_iFrameIndex)
        {
            if (m_iPrevFrameIndexForSound != m_iFrameIndex)
            {
                m_iPrevFrameIndexForSound = m_iFrameIndex;
                if (nullptr != m_pScene)
                    m_pScene->HitPlayer();
            }
        }
        else
        {
            m_iPrevFrameIndexForSound = m_iFrameIndex;
        }
    }

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
RESULT Animator::Ready(string tag, _bool isEnemy, CGameObject* pObj, SceneDungeon* pScene)
{
    m_fAnimSpeed = 1.f;
    m_pScene = pScene;

    m_currentTag = tag;
    if (isEnemy)
        m_pEnemy = dynamic_cast<Enemy*>(pObj);
    else
        m_pPlayer = dynamic_cast<Player*>(pObj);
    
    m_pCurAnimation = CAnimationData::GetInstance()->FindAnimation(tag);
    m_pvecPrevTransforms = GetMatrix();

	return PK_NOERROR;
}

// Create an instance
Animator* Animator::Create(string tag, _bool isEnemy, CGameObject* pObj, SceneDungeon* pScene)
{
    Animator* pInstance = new Animator();
    if (PK_NOERROR != pInstance->Ready(tag, isEnemy, pObj, pScene))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
