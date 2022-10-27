#ifndef _BARREL_H_
#define _BARREL_H_

#include "Define.h"
#include "GameObject.h"
#include "OpenGLDefines.h"

namespace Engine
{
	class CInputDevice;
	class CTransform;
	class CMesh;
	class CShader;
	class CLayer;
	class CTexture;
}
class ConfigurationManager;

class Barrel : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CTransform*			m_pTransform;
	Engine::CMesh*				m_pMesh;
	Engine::CShader*			m_pShader;
	Engine::CTexture*			m_pDiffTexture;

	eBULLET_TYPE				m_eBulletType;

protected:
	explicit Barrel();
	virtual ~Barrel();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetParentTransform(Engine::CTransform*);
private:
	void KeyCheck(const _float&);
	void FireBullet();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
public:
	static Barrel* Create(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
};

#endif //_BARREL_H_