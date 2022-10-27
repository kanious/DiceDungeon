#ifndef _BULLET_H_
#define _BULLET_H_

#include "Define.h"
#include "GameObject.h"
#include "OpenGLDefines.h"

namespace Engine
{
	class CTransform;
	class CMesh;
	class CShader;
}
class ConfigurationManager;

class Bullet : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CTransform*			m_pTransform;
	Engine::CMesh*				m_pMesh;
	Engine::CShader*			m_pShader;

	_float						m_fMass;
	_float						m_fVelocity;
	_float						m_fTimeElapsed;
	_float						m_fForce;
	_float						m_fCoef;

	_float						m_fAngle;
	_uint						m_iBounceCount;

protected:
	explicit Bullet();
	virtual ~Bullet();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	void Boom();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
public:
	static Bullet* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
};

#endif //_BULLET_H_