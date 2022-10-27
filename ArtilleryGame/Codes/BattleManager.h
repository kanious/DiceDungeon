#ifndef _BATTLEMANAGER_H_
#define _BATTLEMANAGER_H_

#include "Define.h"
#include "OpenGLDefines.h"

class Enemy;
class BattleManager
{
	SINGLETON(BattleManager)

private:
	_uint						m_iBulletCount;
	_uint						m_iAllBulletCount;
	Enemy*						m_pEnemy;
	_bool						m_bIsFiring;
	_bool						m_bGameEnd;
	_bool						m_bESCPressed;

private:
	explicit BattleManager();
	~BattleManager();
public:
	void Destroy();

public:
	_bool GetCanFire()					{ return m_bIsFiring || m_bGameEnd; }
	_uint GetAllBulletCount()			{ return m_iAllBulletCount; }
	void SetESCInfo(_bool info)			{ m_bESCPressed = info; }
	void SetIsFiring(_bool fire)		{ m_bIsFiring = fire; }
	void SetEnemy(Enemy* enemy)			{ m_pEnemy = enemy; }
	void AddBulletNum();
	void SubtractBulletNum();
	void BoomLocation(vec3, _float, _float);
	void MakeBoomEffect(vec3, _float);
	void SetNewGame();
};

#endif //_BATTLEMANAGER_H_