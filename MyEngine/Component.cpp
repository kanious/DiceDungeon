#include "pch.h"
#include "Component.h"


USING(Engine)

CComponent::CComponent()
{
}

CComponent::~CComponent()
{
}

void CComponent::Awake()
{
}

void CComponent::Update(const _float& dt)
{
}

void CComponent::Render()
{
}

void CComponent::Destroy()
{
}

void CComponent::Enable()
{
	m_bEnable = true;
}

void CComponent::Disable()
{
	m_bEnable = false;
}

void CComponent::SetObjectType(eCOMPONENT type)
{
	m_eType = type;
}
