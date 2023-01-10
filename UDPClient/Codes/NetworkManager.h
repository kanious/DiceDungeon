#ifndef _NETWORKMANAGER_H_
#define _NETWORKMANAGER_H_

#include "Base.h"
#include <WinSock2.h>
#include "EngineStruct.h"
#include "InputInfo.h"

class Scene3D;
class InputInfo;
class Buffer;

class NetworkManager : public Engine::CBase
{
	SINGLETON(NetworkManager)

private:
	enum NetworkClientState {
		NotInitialized,
		Initialized,
		UnknownError,
	};

private:
	SOCKET					m_socket;
	sockaddr_in				m_addrInfo;
	_int					m_addressSize;
	NetworkClientState		m_eNetwokeState;

	InputInfo				m_userInput;

	Scene3D*				m_pScene;

	_float					m_fRecvElaspedTime;
	_float					m_fSendElaspedTime;
	_float					m_fRecvTime;
	_float					m_fSendTime;


	_long					m_tickNumber;

private:
	explicit NetworkManager();
	~NetworkManager();

public:
	InputInfo* GetUserInput()				{ return &m_userInput; }
	_long GetTickNumber()					{ return m_tickNumber; }
	void SetScene(Scene3D* pScene)			{ m_pScene = pScene; }

public:
	void Destroy();
	RESULT Ready(std::string address, int port);

private:
	_int StartUp();
	_int CreateSocket();
	_int IOCTLsocket();
	_int SetServerInfo(std::string address, int port);

public:
	void Update(const _float& dt);
	void SendTo(const _float& dt);
	void RecvFrom(const _float& dt);

private:
	void Serialize(unsigned int messageId, std::string packetInfo, Buffer& buffer, int& size);
	void Deserialize(char* data, unsigned int length, std::string& packetInfo);
	uint32_t ReadLengthInfo(char* data);
};

#endif //_NETWORKMANAGER_H_