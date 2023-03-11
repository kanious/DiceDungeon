#include "pch.h"
#include "..\Headers\DiceMaster.h"


USING(Engine)
SINGLETON_FUNCTION(CDiceMaster)

CDiceMaster::CDiceMaster()
	: m_iRolledDiceCount(0), m_iAP(0)
{
}

CDiceMaster::~CDiceMaster()
{
}

void CDiceMaster::Destroy()
{
}
