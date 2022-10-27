#ifndef _BOOMEFFECT_H_
#define _BOOMEFFECT_H_

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

class BoomEffect : public Engine::CGameObject
{
private:
	ConfigurationManager*			m_pConfigManager;
	Engine::CTransform*				m_pTransform;
	Engine::CMesh*					m_pMesh;
	Engine::CShader*				m_pShader;
	Engine::CTexture*				m_pDiffTexture;

	_float							m_fRadius;

protected:
	explicit BoomEffect();
	virtual ~BoomEffect();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
public:
	static BoomEffect* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3, _float);
};

#endif //_BOOMEFFECT_H_