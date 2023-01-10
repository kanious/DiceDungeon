#include "BulletInfo.h"

USING(Engine)
USING(glm)

BulletInfo::BulletInfo()
	: m_iState(0), m_fElaspedTime(0.f), m_fTime(2.f)
{
	m_pTransform = CTransform::Create();
}

BulletInfo::~BulletInfo()
{
	SafeDestroy(m_pTransform);
}

void BulletInfo::Update(const _float& dt)
{
	if (0 == m_iState)
		return;

	m_pTransform->Update(0.f);

	m_fElaspedTime += dt;
	if (m_fElaspedTime > m_fTime)
	{
		m_fElaspedTime = 0.f;
		m_iState = 0;
		return;
	}

	vec3 vDir = m_pTransform->GetLookVector();
	m_pTransform->AddPosition(-vDir * dt * 15.f);
}
