#include "pch.h"
#include "Base.h"


USING(Engine)

CBase::CBase()
{
	Awake(); // 자식 Awake()들이 제대로 수행되는지 확인
}

CBase::~CBase()
{
}

void CBase::Awake()
{
}

void CBase::Update(const _float& dt)
{
}

void CBase::Render()
{
}

void CBase::Enable()
{
}

void CBase::Disable()
{
}

void CBase::Destroy()
{
}
