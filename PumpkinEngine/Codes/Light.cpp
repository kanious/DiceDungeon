#include "pch.h"
#include "..\Headers\Light.h"

USING(Engine)

CLight::CLight()
	: m_pInfo(nullptr)
{
}

CLight::~CLight()
{
}

void CLight::Destroy()
{
	if (nullptr != m_pInfo)
		delete m_pInfo;
}

_uint CLight::GetLocation(_int index)
{
	if (0 > index || 8 < index)
		return 0;

	return m_shaderLocation[index];
}

void CLight::SetLocation(_int index, _uint value)
{
	if (0 > index || 8 < index)
		return;

	m_shaderLocation[index] = value;
}

RESULT CLight::Ready(cLightInfo* pInfo)
{
	if (nullptr == pInfo)
		return PK_ERROR_NULLPTR;

	m_pInfo = pInfo;

	return PK_NOERROR;
}

CLight* CLight::Create(cLightInfo* pInfo)
{
	CLight* pInstance = new CLight();
	if (PK_NOERROR != pInstance->Ready(pInfo))
	{
		delete pInstance;
		pInstance = nullptr;
	}
	return pInstance;
}