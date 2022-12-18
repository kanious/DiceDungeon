#ifndef _CLIENTINFO_H_
#define _CLIENTINFO_H_

#include "Define.h"
#include "EngineStruct.h"

class ClientInfo
{
public:
	SOCKET						m_socket;
	bool						m_bConnected;
	unsigned int				m_number;
	Engine::sUserInfo			m_userInfo;

	_ushort						m_iIndex;
	sockaddr_in					m_clientAddr;
	int							m_iClientAddrSize;
	bool						haveInfo;

public:
	explicit ClientInfo()
		: m_socket(INVALID_SOCKET), m_bConnected(false), m_number(0), m_userInfo()
		, m_iIndex(0), m_clientAddr(), m_iClientAddrSize(0), haveInfo(false)
	{}
	~ClientInfo() {}
};

#endif //_CLIENTINFO_H_