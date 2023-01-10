#include "NetworkManager.h"
#include "OpenGLDevice.h"
#include "Define.h"
#include "Scene3D.h"
#include "Buffer.h"
#include "proto\UserInput.pb.h"
#include "proto\GameState.pb.h"

#pragma comment(lib, "Ws2_32.lib")

#include <Windows.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

SINGLETON_FUNCTION(NetworkManager)
USING(Engine)
USING(std)

NetworkManager::NetworkManager()
	: m_socket(INVALID_SOCKET), m_addrInfo(), m_addressSize(0), m_eNetwokeState(NotInitialized)
	, m_pScene(nullptr), m_userInput()
	, m_fRecvElaspedTime(0.f), m_fSendElaspedTime(0.f), m_tickNumber(0)
{
	m_fRecvTime = 1.f / 5.f;
	m_fSendTime = 1.f / 5.f;
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::Destroy()
{
	if (m_eNetwokeState != Initialized)
		return;

	closesocket(m_socket);
	WSACleanup();
}
RESULT NetworkManager::Ready(string address, int port)
{
	_int result = 0;

	printf("  WSA Start Up . . . ");
	result = StartUp();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Create Socket . . . ");
	result = CreateSocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  IOCTL socket . . . ");
	result = IOCTLsocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	m_eNetwokeState = Initialized;

	printf("  Set Server Information . . . ");
	result = SetServerInfo(address, port);
	if (result != 0)
		return result;
	printf("OK!\n");

	return PK_NOERROR;
}

_int NetworkManager::StartUp()
{
	WSAData wsaData;

	_int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup FAILED : %d\n", result);
		return result;
	}

	return 0;
}

_int NetworkManager::CreateSocket()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET) 
	{
		printf("FAILED : %d\n", WSAGetLastError());
		WSACleanup();
		m_eNetwokeState = UnknownError;
		return 1;
	}
	return 0;
}

_int NetworkManager::IOCTLsocket()
{ 
	DWORD NonBlock = 1;
	_int result = ioctlsocket(m_socket, FIONBIO, &NonBlock);
	if (result == SOCKET_ERROR)
	{
		printf("FAILED : %d\n", WSAGetLastError());
		WSACleanup();
		m_eNetwokeState = UnknownError;
		return 1;
	}

	return 0;
}

_int NetworkManager::SetServerInfo(string address, int port)
{
	if (Initialized != m_eNetwokeState)
		return 1;

	m_addrInfo.sin_family = AF_INET;
	m_addrInfo.sin_port = htons(port);
	int result = inet_pton(AF_INET, address.c_str(), &m_addrInfo.sin_addr.s_addr);
	if (result == 0)
	{
		printf("Warning Not a valid IP string : %d\n", WSAGetLastError());
		m_eNetwokeState = UnknownError;
		return 1;
	}
	else if (result == SOCKET_ERROR)
	{
		printf("FAILED : %d\n", WSAGetLastError());
		m_eNetwokeState = UnknownError;
		return 1;
	}

	m_addressSize = sizeof(m_addrInfo);

	return 0;
}

void NetworkManager::Update(const _float& dt)
{
}

void NetworkManager::SendTo(const _float& dt)
{
	if (m_eNetwokeState != Initialized)
		return;

	m_fSendElaspedTime += dt;
	if (m_fSendElaspedTime < m_fSendTime)
		return;
	m_fSendElaspedTime = 0.f;

	Buffer buffer(0);
	UserInput buf;
	buf.set_messageid(Msg_UserInput);
	buf.set_input(m_userInput.Setting());
	buf.set_tick_number(m_tickNumber);
	std::string serialize;
	buf.SerializeToString(&serialize);

	int size = 0;
	Serialize(Msg_UserInput, serialize, buffer, size);

	_int result = sendto(m_socket, (char*)&(buffer.m_data[0]), size, 0, (SOCKADDR*)&m_addrInfo, m_addressSize);
	if (result == SOCKET_ERROR)
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
			printf("Socket Error (SendTo) : %d\n", WSAGetLastError());
		m_eNetwokeState = UnknownError;
		return;
	}

	++m_tickNumber;
}

void NetworkManager::RecvFrom(const _float& dt)
{
	if (m_eNetwokeState != Initialized)
		return;

	m_fRecvElaspedTime += dt;
	if (m_fRecvElaspedTime < m_fRecvTime)
		return;
	m_fRecvElaspedTime = 0.f;

	m_pScene->ResetInput();

	char buf[CHUNK_SIZE];

	_int result = recvfrom(m_socket, buf, CHUNK_SIZE, 0, (SOCKADDR*)&m_addrInfo, &m_addressSize);
	if (result == SOCKET_ERROR)
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
			printf("Socket Error (RecvFrom) : %d\n", WSAGetLastError());
		return;
	}

	std::string packetInfo;
	Deserialize(buf, result, packetInfo);

	GameState packet;
	_bool success = packet.ParseFromString(packetInfo);
	if (success)
	{
		m_pScene->SetGameState(packet);
	}
}

void NetworkManager::Serialize(unsigned int messageId, std::string packetInfo, Buffer& buffer, int& size)
{
	size = sizeof(unsigned int)
		+ sizeof(unsigned int)
		+ sizeof(unsigned int)
		+ packetInfo.size();

	buffer.Resize(size);
	buffer.WriteInt(size);
	buffer.WriteInt(messageId);
	buffer.WriteInt(packetInfo.size());
	buffer.WriteString(packetInfo);
}

void NetworkManager::Deserialize(char* data, unsigned int length, std::string& packetInfo)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);

	unsigned int header_length = buffer.ReadInt();
	unsigned int header_messageId = buffer.ReadInt();

	int size = buffer.ReadInt();
	packetInfo = buffer.ReadString(size);
}

uint32_t NetworkManager::ReadLengthInfo(char* data)
{
	uint32_t value = 0;
	value = data[3];
	value |= data[2] << 8;
	value |= data[1] << 16;
	value |= data[0] << 24;
	return value;
}