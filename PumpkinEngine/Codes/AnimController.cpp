#include "pch.h"
#include "..\Headers\AnimController.h"
#include "..\Headers\Animation.h"
#include <functional>


USING(Engine)
USING(std)
USING(glm)

CAnimController::CAnimController()
	: m_pCurAnimation(nullptr), m_iFrameIndex(0), m_fCurTime(0.f), m_curTag("")
	, m_fAnimSpeed(1.f), m_bReverse(false), m_bPause(false), m_bIsPlaying(false)
	, m_bChangeAnimation(false), m_pvecPrevTransforms(nullptr), m_prevTag("")
	, m_fWeight(0.3f), m_fLastAccumWeight(0.f), m_fTotalAccumWeight(0.f), m_iPrevFrameIndex(0)
{
}

CAnimController::~CAnimController()
{
}

void CAnimController::Destroy()
{
	m_pCurAnimation = nullptr;
	m_iFrameIndex = 0;
}

vector<mat4x4>* CAnimController::GetMatrix()
{
	if (nullptr != m_pCurAnimation)
		return m_pCurAnimation->GetMatrix(m_iFrameIndex);

	return nullptr;
}

vector<mat4x4>* CAnimController::GetPrevMatrix()
{
	return m_pvecPrevTransforms;
}

void CAnimController::FrameMove(const _float& dt)
{
	if (m_bPause)
		return;

	if (nullptr != m_pCurAnimation)
	{
		m_fCurTime += dt * m_fAnimSpeed;
		m_pCurAnimation->FrameMove(m_fCurTime, m_iFrameIndex, m_bReverse
			, bind(&CAnimController::AnimationEndEvent, this));
	}
}

void CAnimController::SetNextFrame()
{
	if (m_iFrameIndex == m_pCurAnimation->GetAnimationLength() - 1)
		m_iFrameIndex = 0;
	else
		m_iFrameIndex++;

	m_fCurTime = 0.f;
}

void CAnimController::SetPreviousFrame()
{
	if (m_iFrameIndex == 0)
		m_iFrameIndex = m_pCurAnimation->GetAnimationLength() - 1;
	else
		m_iFrameIndex--;

	m_fCurTime = 0.f;
}

void CAnimController::SetReverse()
{
	m_bReverse = !m_bReverse;
}

void CAnimController::SetPause()
{
	m_bPause = !m_bPause;
}

void CAnimController::StopAnimation()
{

}

void CAnimController::ResetAnimation()
{
	m_fCurTime = 0;
	m_iFrameIndex = 0;
}

void CAnimController::InitBlendingVariables()
{
	m_bChangeAnimation = false;
	m_fLastAccumWeight = 0.f;
	m_fTotalAccumWeight = 0.f;
	m_prevTag = m_curTag;
	m_iPrevFrameIndex = 0;
}

void CAnimController::StartBlending()
{
	m_pvecPrevTransforms = GetMatrix();
	InitBlendingVariables();
	m_bChangeAnimation = true;
}

void CAnimController::BlendingMove()
{
	if (m_iPrevFrameIndex != m_iFrameIndex)
	{
		m_fLastAccumWeight = m_fWeight * (1.f - m_fTotalAccumWeight);
		m_iPrevFrameIndex = m_iFrameIndex;
	}
	else
		m_fLastAccumWeight = 0.f;

	m_fTotalAccumWeight += m_fLastAccumWeight;
}
