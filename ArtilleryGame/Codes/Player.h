#ifndef _PLAYER_H_
#define _PLAYER_H_

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
class Barrel;

class Player : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CTransform*			m_pTransform;
	Engine::CMesh*				m_pMesh;
	Engine::CShader*			m_pShader;
	Engine::CTexture*			m_pDiffTexture;
private:
	Barrel*						m_pBarrel;

protected:
	explicit Player();
	virtual ~Player();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void LookAt(vec3);

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
public:
	static Player* Create(eSCENETAG, eLAYERTAG, eOBJTAG, Engine::CLayer*, vec3, vec3, vec3);
};

#endif //_PLAYER_H_