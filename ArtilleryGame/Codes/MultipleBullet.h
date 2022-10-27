#ifndef _MULTIPLEBULLET_H_
#define _MULTIPLEBULLET_H_

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

class MultipleBullet : public Engine::CGameObject
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
	_float						m_fChildCreateTime;
	_float						m_fExplosionRadius;
	_float						m_fExplosionPower;

	vec3						m_vForce;
	vec3						m_vAcc;
	vec3						m_vFirstPos;
	_float						m_fTimeElapsed;
	_float						m_fAngle;
	_uint						m_iCount;
	_bool						m_bAlreadyMadeChild;

protected:
	explicit MultipleBullet();
	virtual ~MultipleBullet();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetChildStatus(_uint, vec3);
	void SetForce(vec3);
private:
	void ShootChildBullet();
	void Boom();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
public:
	static MultipleBullet* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
};

#endif //_MULTIPLEBULLET_H_