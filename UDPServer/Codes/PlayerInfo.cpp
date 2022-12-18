#include "PlayerInfo.h"

USING(Engine)

PlayerInfo::PlayerInfo()
	: m_iState(0), m_tickNumber(0)
{
	m_pTransform = CTransform::Create();
}

PlayerInfo::~PlayerInfo()
{
 	SafeDestroy(m_pTransform);
}

void PlayerInfo::Forward(const _float& dt)
{
	m_pTransform->AddPositionZ(-1.f);
}

void PlayerInfo::Backward(const _float& dt)
{
	m_pTransform->AddPositionZ(1.f);
}

void PlayerInfo::Left(const _float& dt)
{
	m_pTransform->AddPositionX(-1.f);
}

void PlayerInfo::Right(const _float& dt)
{
	m_pTransform->AddPositionX(1.f);
}

void PlayerInfo::TurnLeft(const _float& dt)
{
}

void PlayerInfo::TurnRight(const _float& dt)
{
}

void PlayerInfo::Fire()
{
}

void PlayerInfo::Respawn()
{
}
