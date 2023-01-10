#include "PlayerInfo.h"
#include "BulletInfo.h"

USING(Engine)
USING(glm)

PlayerInfo::PlayerInfo()
	: m_iState(0), m_tickNumber(0)
{
	m_pTransform = CTransform::Create();
}

PlayerInfo::~PlayerInfo()
{
 	SafeDestroy(m_pTransform);
}

void PlayerInfo::UpdateTransform()
{
	m_pTransform->Update(0.f);
}

void PlayerInfo::Forward(const _float& dt)
{
	vec3 vDir = m_pTransform->GetLookVector();
	m_pTransform->AddPosition(-vDir * dt * 10.f);
}

void PlayerInfo::Backward(const _float& dt)
{
	vec3 vDir = m_pTransform->GetLookVector();
	m_pTransform->AddPosition(vDir * dt * 10.f);
}

void PlayerInfo::Left(const _float& dt)
{
	vec3 vDir = m_pTransform->GetRightVector();
	m_pTransform->AddPosition(-vDir * dt * 10.f);
}

void PlayerInfo::Right(const _float& dt)
{
	vec3 vDir = m_pTransform->GetRightVector();
	m_pTransform->AddPosition(vDir * dt * 10.f);
}

void PlayerInfo::TurnLeft(const _float& dt)
{
	m_pTransform->AddRotationY(dt * 50.f);
}

void PlayerInfo::TurnRight(const _float& dt)
{
	m_pTransform->AddRotationY(-dt * 50.f);
}

void PlayerInfo::Fire(BulletInfo* myBullet)
{
	if (1 != m_iState)
		return;

	m_iState = 2;
	myBullet->m_iState = 1;
	vec3 vPos = m_pTransform->GetPosition();
	vec3 vRot = m_pTransform->GetRotation();
	myBullet->m_pTransform->SetPosition(vPos);
	myBullet->m_pTransform->SetRotation(vRot);
}

void PlayerInfo::Respawn()
{
	if (0 != m_iState)
		return;

	m_iState = 1;
}
