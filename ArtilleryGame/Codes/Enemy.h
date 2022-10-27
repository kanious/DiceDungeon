#ifndef _ENEMY_H_
#define _ENEMY_H_

#include "Define.h"
#include "GameObject.h"
#include "OpenGLDefines.h"

namespace Engine
{
	class CTransform;
	class CMesh;
	class CShader;
	class CTexture;
}
class ConfigurationManager;

class Enemy : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CTransform*			m_pTransform;
	Engine::CMesh*				m_pMesh;
	Engine::CShader*			m_pShader;
	Engine::CTexture*			m_pDiffTexture;
	
	vec3						m_vVelocity;
	_float						m_fMass;
	_float						m_fInvMass;
	_float						m_fCoef;

	vec3						m_vForce;
	vec3						m_vAcc;
	vec3						m_vDir;
	_float						m_fPower;

	_int						m_iMaxHP;
	_int						m_iCurrentHP;

protected:
	explicit Enemy();
	virtual ~Enemy();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void LookAt(vec3);
	vec3 GetPosition();
	_int GetMaxHP()				{ return m_iMaxHP; }
	_int GetCurrentHP()			{ return m_iCurrentHP; }
	void Hit(vec3, _float);
	void SetDamage(_int);
private:
	void CheckWall();
	vec3 GetReflectVector(vec3, vec3);

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
public:
	static Enemy* Create(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
};

#endif //_ENEMY_H_