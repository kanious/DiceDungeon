#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "Define.h"

namespace Engine
{
	class CGameMaster;
	class CTimer;
	class CInputDevice;
	class COpenGLDevice;
}
class ConfigurationManager;
class ComponentManager;

class Client
{
private:
	Engine::CGameMaster*		m_pGameMaster;
	Engine::CTimer*				m_pTimer;
	Engine::CInputDevice*		m_pInputDevice;
	Engine::COpenGLDevice*		m_pOpenGLDevice;
	ConfigurationManager*		m_pConfigurationManager;

	_int						m_iFPS;

protected:
	explicit Client();
	virtual ~Client();

public:
	void Loop();
	void Destroy();

public:
	static Client* Create();
private:
	RESULT Ready();
};

#endif //_CLIENT_H_