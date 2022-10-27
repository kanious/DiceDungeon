#ifndef _DIVIDEDBULLET_H_
#define _DIVIDEDBULLET_H_

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

class DividedBullet : public Engine::CGameObject
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
	_float						m_fExplosionRadius;
	_float						m_fExplosionPower;

	vec3						m_vForce;
	vec3						m_vAcc;
	_float						m_fTimeElapsed;
	_float						m_fAngle;
	_bool						m_bParent;

	_float						m_fRandTime;
	_bool						m_bDelay;


protected:
	explicit DividedBullet();
	virtual ~DividedBullet();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetChildStatus(_float, vec3);
	void SetForce(vec3);
	void SetExplosionPower(_float);
private:
	void ShootChildBullet(_uint);
	void Boom();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float, _bool);
public:
	static DividedBullet* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float, _bool);
};

#endif //_DIVIDEDBULLET_H_