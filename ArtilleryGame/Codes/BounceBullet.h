#ifndef _BOUONCEBULLET_H_
#define _BOUONCEBULLET_H_

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

class BounceBullet : public Engine::CGameObject
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
	_uint						m_iBounceCount;

protected:
	explicit BounceBullet();
	virtual ~BounceBullet();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetForce(vec3);
private:
	void Boom();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
public:
	static BounceBullet* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
};

#endif //_BOUONCEBULLET_H_