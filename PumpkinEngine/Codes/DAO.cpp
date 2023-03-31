#include "pch.h"
#include "../Headers/DAO.h"

USING(Engine)
USING(std)

CDAO::CDAO()
{
}

CDAO::~CDAO()
{
}

void CDAO::Destroy()
{
}

void CDAO::setHighScore(_int playerId, _int score)
{
}

_int CDAO::getHighScore(_int playerId)
{
	return _int();
}

RESULT CDAO::Ready()
{
	return PK_NOERROR;
}

CDAO* CDAO::Create()
{
	CDAO* pInstance = new CDAO();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
