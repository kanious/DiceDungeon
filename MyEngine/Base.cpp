#include "pch.h"
#include "Base.h"


USING(Engine)

CBase::CBase()
{
	Awake(); // �ڽ� Awake()���� ����� ����Ǵ��� Ȯ��
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
