#ifndef _DICEMASTER_H_
#define _DICEMASTER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;

// Dice managing class
class ENGINE_API CDiceMaster : public CBase
{
	SINGLETON(CDiceMaster)
private:
	_uint		m_iRolledDiceCount;
	_uint		m_iAP;

private:
	explicit CDiceMaster();
	virtual ~CDiceMaster();
	// Call instead of destructor to manage class internal data
	void Destroy();

public:
	void SetRolledDiceCount(_uint value)	{ m_iRolledDiceCount = value;}
	void SetAP(_uint value)					{ m_iAP = value; }
	_uint GetRolledDiceCount()				{ return m_iRolledDiceCount; }
	_uint GetAP()							{ return m_iAP; }
public:
	// Add AP
	void AddAP(_uint value) { m_iAP += value; }
	// Subtract AP
	void MinusAP(_uint value) { m_iAP -= value; }
};

NAMESPACE_END

#endif //_DICEMASTER_H_