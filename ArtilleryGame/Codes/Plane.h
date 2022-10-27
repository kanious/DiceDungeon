#ifndef _PLANE_H_
#define _PLANE_H_

#include "Define.h"
#include "GameObject.h"
#include "OpenGLDefines.h"

namespace Engine
{
	class CTransform;
	class CMeshSquare;
	class CShader;
	class CTexture;
}
class ConfigurationManager;

class Plane : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CTransform*			m_pTransform;
	Engine::CMeshSquare*		m_pMeshSqaure;
	Engine::CShader*			m_pShader;
	Engine::CTexture*			m_pDiffTexture;

protected:
	explicit Plane();
	virtual ~Plane();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3);
public:
	static Plane* Create(eSCENETAG, eLAYERTAG, eOBJTAG, vec3, vec3, vec3);
};

#endif //_PLANE_H_