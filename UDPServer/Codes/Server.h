#ifndef _SERVER_H_
#define _SERVER_H_

#include "Define.h"
#include "EngineStruct.h"
#include "PlayerInfo.h"
#include "InputInfo.h"

class ClientInfo;
class BulletInfo;
class Buffer;
class Server
{
private:
	struct sockaddr_in			m_recvAddr;
	SOCKET						m_socket;

	unsigned int				UserID;

public:
	ClientInfo*					m_curClientInfo;

	typedef std::unordered_map<_ushort, ClientInfo*> MAP_CLIENT;
	MAP_CLIENT					m_mapClients;

	PlayerInfo*					m_playerInfo[4];
	BulletInfo*					m_bulletInfo[4];
	InputInfo					m_inputInfo[4];

	_float						m_fRecvElaspedTime;
	_float						m_fSendElaspedTime;
	_float						m_fRecvTime;
	_float						m_fSendTime;

	int test;

public:
	explicit Server();
	~Server();

public:
	int ServerOpen();
private:
	int StartUp();
	int CreateSocket();
	int Bind();
	int RecvFromClient();
public:
	void RecvFrom(const _float& dt);
	void Update(const _float& dt);
	void SendTo(const _float& dt);
	void Close();

private:
	void Serialize(unsigned int messageId, std::string packetInfo, Buffer& buffer, int& size);
	void Deserialize(char* data, unsigned int length, std::string& packetInfo);
	uint32_t ReadLengthInfo(char* data);
};

#endif //_SERVER_H_