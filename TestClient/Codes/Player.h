#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CInputDevice;
	class CTransform;
	class CMesh;
	class CShader;
}
class ConfigurationManager;

class Player : public Engine::CGameObject
{
private:
	ConfigurationManager*		m_pConfigManager;
	Engine::CInputDevice*		m_pInputDevice;
	Engine::CTransform*			m_pTransform;
	Engine::CMesh*				m_pMesh;
	Engine::CShader*			m_pShader;

protected:
	explicit Player();
	virtual ~Player();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	void KeyCheck(const _float&);

private:
	RESULT Ready(eSCENETAG, eLAYERTAG, eOBJTAG);
public:
	static Player* Create(eSCENETAG, eLAYERTAG, eOBJTAG);
};

#endif //_PLAYER_H_