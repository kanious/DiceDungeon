#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "GameObject.h"
#include "Enums.h"

namespace Engine
{
	class CMesh;
}
class RobotController;

class Robot : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	Engine::CMesh*					m_pWeaponMesh;
	std::string						m_weaponType;
	RobotController*				m_pController;
	_uint							m_iRobotNumber;
	eWeaponType						m_eWeaponType;

	Robot*							m_pTarget;
	_bool							m_bTargetConfirmed;
	_bool							m_bWaitingCoolTime;

	_bool							m_bGlobalCoolTime;
	_float							m_fGlobalCoolTime;
	_float							m_fGlobalCoolTimeElapse;

	_bool							m_bWeaponCoolTime;
	_float							m_fWeaponCoolTime;
	_float							m_fWeaponCoolTimeElapse;

	_bool							m_bWaitingProjectile;

	_int							m_iHP;
	_bool							m_bRetired;

	_float							m_fInitGravity;
	_float							m_fCurrentGravity;

private:
	explicit Robot();
	virtual ~Robot();
public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string weaponType);

public:
	_uint GetRobotNumber()							{ return m_iRobotNumber; }
	void SetRobotNumder(_uint num)					{ m_iRobotNumber = num; }
	_bool GetWaitingProjectileStatus()				{ return m_bWaitingProjectile; }
	void SetWaitingProjectileStatus(_bool value)	{ m_bWaitingProjectile = value; }
	_int GetHP()									{ return m_iHP; }
	void SetHP(_int value)							{ m_iHP = value; }
	_bool GetRetired()								{ return m_bRetired; }
	Robot* GetTarget();
	void LostTarget();
	void Damaged(_int value);
	void ProjectileResult(_bool success, _bool closer = false);

private:
	void RequestTarget();
	void RequestEyeOnTarget();
	void RequestShoot();

public:
	static Robot* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string weaponType);
};


#endif //_ROBOT_H_