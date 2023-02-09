#include "AnimationManager.h"
#include "Animator.h"
#include "InputDevice.h"
#include "Player.h"


SINGLETON_FUNCTION(AnimationManager)
USING(Engine)
USING(std)

AnimationManager::AnimationManager()
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_vecAnimators.clear();

	Ready();
}

AnimationManager::~AnimationManager()
{
}

void AnimationManager::Destroy()
{
	SafeDestroy(m_pInputDevice);

	for (int i = 0; i < m_vecAnimators.size(); ++i)
		SafeDestroy(m_vecAnimators[i]);
	m_vecAnimators.clear();
}

RESULT AnimationManager::Ready()
{
	return PK_NOERROR;
}

void AnimationManager::Update(const _float& dt)
{
	KeyCheck();

	for (int i = 0; i < m_vecAnimators.size(); ++i)
		m_vecAnimators[i]->FrameMove(dt);
}

void AnimationManager::AddAnimator(Animator* pAnimator)
{
	m_vecAnimators.push_back(pAnimator);
}

void AnimationManager::SetTargetAnimator(Player* pPlayer)
{
	if (nullptr != m_pTarget)
		m_pTarget->Deselect();

	m_pTarget = pPlayer;
}

void AnimationManager::KeyCheck()
{
	//Reverse All
	static _bool isRDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_R))
	{
		if (!isRDown)
		{
			isRDown = true;
			for (int i = 0; i < m_vecAnimators.size(); ++i)
				m_vecAnimators[i]->SetReverse();
		}
	}
	else
		isRDown = false;

	// Next Sequence
	static _bool isRightArrowDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_RIGHT))
	{
		if (!isRightArrowDown)
		{
			isRightArrowDown = true;
			if (nullptr == m_pTarget)
			{
				for (int i = 0; i < m_vecAnimators.size(); ++i)
					m_vecAnimators[i]->SetNextFrame();
			}
			else
				m_pTarget->GetAnimator()->SetNextFrame();
		}
	}
	else
		isRightArrowDown = false;

	// Previous Sequence
	static _bool isLeftArrowDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_LEFT))
	{
		if (!isLeftArrowDown)
		{
			isLeftArrowDown = true;
			if (nullptr == m_pTarget)
			{
				for (int i = 0; i < m_vecAnimators.size(); ++i)
					m_vecAnimators[i]->SetPreviousFrame();
			}
			else
				m_pTarget->GetAnimator()->SetPreviousFrame();
		}
	}
	else
		isLeftArrowDown = false;

	// Play/Pause Toggle
	static _bool isSpaceDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_SPACE))
	{
		if (!isSpaceDown)
		{
			isSpaceDown = true;
			if (nullptr == m_pTarget)
			{
				for (int i = 0; i < m_vecAnimators.size(); ++i)
					m_vecAnimators[i]->SetPause();
			}
			else
				m_pTarget->GetAnimator()->SetPause();
		}
	}
	else
		isSpaceDown = false;

	// Speed Check
	static _bool isSpeedOneDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_1))
	{
		if (!isSpeedOneDown)
		{
			isSpeedOneDown = true;
			SetAnimationSpeed(1.f);
		}
	}
	else
		isSpeedOneDown = false;

	static _bool isSpeedTwoDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_2))
	{
		if (!isSpeedTwoDown)
		{
			isSpeedTwoDown = true;
			SetAnimationSpeed(2.f);
		}
	}
	else
		isSpeedTwoDown = false;

	static _bool isSpeedThreeDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_3))
	{
		if (!isSpeedThreeDown)
		{
			isSpeedThreeDown = true;
			SetAnimationSpeed(3.f);
		}
	}
	else
		isSpeedThreeDown = false;

	static _bool isSpeedFourDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_4))
	{
		if (!isSpeedFourDown)
		{
			isSpeedFourDown = true;
			SetAnimationSpeed(4.f);
		}
	}
	else
		isSpeedFourDown = false;


	static _bool isSpeedFiveDown = false;
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_5))
	{
		if (!isSpeedFiveDown)
		{
			isSpeedFiveDown = true;
			SetAnimationSpeed(5.f);
		}
	}
	else
		isSpeedFiveDown = false;
}

void AnimationManager::SetAnimationSpeed(_float speed)
{
	if (nullptr == m_pTarget)
	{
		for (int i = 0; i < m_vecAnimators.size(); ++i)
			m_vecAnimators[i]->SetAnimationSpeed(speed);
	}
	else
		m_pTarget->GetAnimator()->SetAnimationSpeed(speed);
}
