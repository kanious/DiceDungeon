#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "EngineDefines.h"

namespace Engine
{
	class CGameMaster;
	class CTimer;
	class COpenGLDevice;
	class CInputDevice;
}
class Client
{
private:
	Engine::CGameMaster*			m_pGameMaster;
	Engine::CTimer*					m_pTimer;
	Engine::COpenGLDevice*			m_pGraphicDevice;
	Engine::CInputDevice*			m_pInputDevice;
	_int m_iFPS;

public:
	explicit Client();
	~Client();

public:
	void Loop();
	RESULT Ready();
	void Destroy();
};

#endif //_CLIENT_H_