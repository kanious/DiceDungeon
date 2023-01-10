#include "Server.h"
#include "ClientInfo.h"
#include "BulletInfo.h"
#include "Buffer.h"
#include "proto\UserInput.pb.h"
#include "proto\GameState.pb.h"
#include <string>


USING(Engine)
USING(glm)

const int SHORT_SIZE = 2;
const int INTEGER_SIZE = 4;
Server::Server()
	: m_recvAddr(), m_socket(INVALID_SOCKET), UserID(0)
	, m_fRecvElaspedTime(0.f), m_fSendElaspedTime(0.f)
{
	m_curClientInfo = new ClientInfo();
	m_fRecvTime = 1.f / 5.f;
	m_fSendTime = 1.f / 5.f;

	m_mapClients.clear();

	for (int i = 0; i < 4; ++i)
	{
		m_playerInfo[i] = new PlayerInfo();
		m_bulletInfo[i] = new BulletInfo();
	}
}

Server::~Server()
{
}

int Server::ServerOpen()
{
	int result = 0;

	printf("  Server Starting . . . ");
	result = StartUp();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Create Socket . . . ");
	result = CreateSocket();
	if (result != 0)
		return result;
	printf("OK!\n");

	printf("  Bind Socket . . . ");
	result = Bind();
	if (result != 0)
		return result;
	printf("OK!\n");

	return result;
}

int Server::StartUp()
{
	WSAData wsaData;
	int result = 0;
	
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup FAILED : %d\n", result);
		return result;
	}

	return 0;
}

int Server::CreateSocket()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_socket == INVALID_SOCKET)
	{
		printf("Socket Create FAILED : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	DWORD NonBlock = 1;
	int result = ioctlsocket(m_socket, FIONBIO, &NonBlock);
	if (result != 0)
	{
		printf("ioctlsocket FAILED : %d\n", WSAGetLastError());
		WSACleanup();
		return result;
	}

	return 0;
}

int Server::Bind()
{
	m_recvAddr.sin_family = AF_INET;
	m_recvAddr.sin_port = htons(DEFAULT_PORT);
	m_recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int result = bind(m_socket, (SOCKADDR*)&m_recvAddr, sizeof(m_recvAddr));
	if (result == SOCKET_ERROR)
	{
		printf("Socket Bind FAILED : %d\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return result;
	}

	m_curClientInfo->m_iClientAddrSize = sizeof(m_curClientInfo->m_clientAddr);
	m_curClientInfo->haveInfo = false;

	return 0;
}

int Server::RecvFromClient()
{
	char buf[CHUNK_SIZE];

	int result = recvfrom(m_socket, buf, CHUNK_SIZE, 0, (SOCKADDR*)&m_curClientInfo->m_clientAddr
		, &m_curClientInfo->m_iClientAddrSize);
	if (result == SOCKET_ERROR)
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
			printf("Socket Error (RecvFrom) : %d\n", WSAGetLastError());
		m_curClientInfo->haveInfo = false;

		MAP_CLIENT::iterator iter;
		for (iter = m_mapClients.begin(); iter != m_mapClients.end(); ++iter)
			iter->second->haveInfo = false;

		return 0;
	}

	m_curClientInfo->haveInfo = true;

	_ushort port = m_curClientInfo->m_clientAddr.sin_port;
	MAP_CLIENT::iterator iter = m_mapClients.find(port);
	if (iter == m_mapClients.end())
	{
		_ushort nextIdx = m_mapClients.size();

		ClientInfo* newClient = new ClientInfo();
		newClient->m_iIndex = nextIdx;
		newClient->m_clientAddr = m_curClientInfo->m_clientAddr;
		newClient->m_iClientAddrSize = m_curClientInfo->m_iClientAddrSize;
		newClient->haveInfo = m_curClientInfo->haveInfo;
		m_mapClients.insert(MAP_CLIENT::value_type(port, newClient));

		m_playerInfo[nextIdx]->m_iState = 1;
	}

	iter = m_mapClients.find(port);
	ClientInfo* pClient = iter->second;

	_ushort playerIndex = m_mapClients[port]->m_iIndex;

	std::string packetInfo;
	uint32_t fullLength = ReadLengthInfo(buf);

	Deserialize(buf, fullLength, packetInfo);

	UserInput packet;
	_bool success = packet.ParseFromString(packetInfo);
	if (success)
	{
		m_playerInfo[playerIndex]->m_tickNumber = packet.tick_number();
		m_inputInfo[playerIndex].Calculate(packet.input());
	}

	return 1;
}

void Server::RecvFrom(const _float& dt)
{
	m_fRecvElaspedTime += dt;
	if (m_fRecvElaspedTime < m_fRecvTime)
		return;

	m_fRecvElaspedTime = 0.f;

	_bool readFinished = false;
	while (!readFinished)
	{
		_int recvResult = RecvFromClient();

		if (0 == recvResult)
			readFinished = true;
	}
}

void Server::Update(const _float& dt)
{
	for (int i = 0; i < 4; ++i)
	{
		if (0 == m_bulletInfo[i]->m_iState)
			continue;

		m_bulletInfo[i]->Update(dt);
	}

	for (int i = 0; i < 4; ++i)
	{
		if (0 == m_playerInfo[i]->m_iState)
		{
			if (m_inputInfo[i].respawn)
			{
				m_playerInfo[i]->Respawn();
				m_inputInfo[i].respawn = false;
			}
			continue;
		}

		m_playerInfo[i]->UpdateTransform();

		if (m_inputInfo[i].forward)
			m_playerInfo[i]->Forward(dt);

		if (m_inputInfo[i].backward)
			m_playerInfo[i]->Backward(dt);

		if (m_inputInfo[i].left)
			m_playerInfo[i]->Left(dt);

		if (m_inputInfo[i].right)
			m_playerInfo[i]->Right(dt);

		if (m_inputInfo[i].turnLeft)
			m_playerInfo[i]->TurnLeft(dt);

		if (m_inputInfo[i].turnRight)
			m_playerInfo[i]->TurnRight(dt);
		
		if (m_inputInfo[i].fire)
		{
			m_playerInfo[i]->Fire(m_bulletInfo[i]);
			m_inputInfo[i].fire = false;
		}

		vec3 vPlayerPos = m_playerInfo[i]->GetPosition();
		for (int j = 0; j < 4; ++j)
		{
			if (i == j || 0 == m_bulletInfo[j]->m_iState)
				continue;

			vec3 vBulletPos = m_bulletInfo[j]->GetPosition();
			_float dist = distance(vPlayerPos, vBulletPos);
			if (dist < 3.f)
			{
				m_playerInfo[i]->m_iState = 0;
				m_bulletInfo[j]->m_iState = 0;
				if (2 == m_playerInfo[j]->m_iState)
					m_playerInfo[j]->m_iState = 1;
			}
		}
	}
}

void Server::SendTo(const _float& dt)
{
	m_fSendElaspedTime += dt;
	if (m_fSendElaspedTime < m_fSendTime)
		return;

	m_fSendElaspedTime = 0.f;

	GameState buf;
	for (int i = 0; i < 4; ++i)
	{
		Object* player = buf.add_players();
		PlayerInfo* info = m_playerInfo[i];
		BulletInfo* myBullet = m_bulletInfo[i];

		info->m_tickNumber++;
		player->set_tick_number(info->m_tickNumber);
		if (2 == info->m_iState)
		{
			if (0 == myBullet->m_iState)
				info->m_iState = 1;
		}
		player->set_state(info->m_iState);
		vec3 vPos = info->GetPosition();
		player->set_posx(vPos.x);
		player->set_posy(vPos.y);
		player->set_posz(vPos.z);
		player->set_roty(info->GetRotationY());

		Object* bullet = buf.add_bullets();
		bullet->set_tick_number(0);
		bullet->set_state(myBullet->m_iState);
		vPos = myBullet->GetPosition();
		bullet->set_posx(vPos.x);
		bullet->set_posy(vPos.y);
		bullet->set_posz(vPos.z);
		bullet->set_roty(myBullet->GetRotationY());
	}

	buf.set_messageid(Msg_GameState);

	MAP_CLIENT::iterator iter;
	for (iter = m_mapClients.begin(); iter != m_mapClients.end(); ++iter)
	{
		ClientInfo* pClient = iter->second;
		_ushort index = pClient->m_iIndex;

		Buffer buffer(0);
		buf.set_clientid(index);
		std::string serialize;
		buf.SerializeToString(&serialize);

		int size = 0;
		Serialize(Msg_GameState, serialize, buffer, size);

		int result = sendto(m_socket, (char*)&(buffer.m_data[0]), size, 0, (SOCKADDR*)&pClient->m_clientAddr
			, pClient->m_iClientAddrSize);

		if (result == SOCKET_ERROR)
		{
			if (WSAEWOULDBLOCK != WSAGetLastError())
				printf("Socket Error (SendTo) : %d\n", WSAGetLastError());
			pClient->haveInfo = false;
			return;
		}
	}

	++test;
	std::cout << test << ". ";
}

void Server::Close()
{
	closesocket(m_socket);
	WSACleanup();

	MAP_CLIENT::iterator iter;
	for (iter = m_mapClients.begin(); iter != m_mapClients.end(); ++iter)
		delete iter->second;

	m_mapClients.clear();

	for (int i = 0; i < 4; ++i)
	{
		delete m_playerInfo[i];
		delete m_bulletInfo[i];
	}

	delete m_curClientInfo;
}

void Server::Serialize(unsigned int messageId, std::string packetInfo, Buffer& buffer, int& size)
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

void Server::Deserialize(char* data, unsigned int length, std::string& packetInfo)
{
	Buffer buffer(length);
	memcpy(&(buffer.m_data[0]), data, length);

	unsigned int header_length = buffer.ReadInt();
	unsigned int header_messageId = buffer.ReadInt();

	int size = buffer.ReadInt();
	packetInfo = buffer.ReadString(size);
}

uint32_t Server::ReadLengthInfo(char* data)
{
	uint32_t value = 0;
	value = data[3];
	value |= data[2] << 8;
	value |= data[1] << 16;
	value |= data[0] << 24;
	return value;
}
