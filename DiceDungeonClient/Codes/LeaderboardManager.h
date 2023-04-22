#ifndef _LEADERBOARDMANAGER_H_
#define _LEADERBOARDMANAGER_H_

#include "Base.h"
#include "Leaderboard.h"
#include "leaderboard_types.h"
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <map>

USING(::apache::thrift)
USING(::apache::thrift::protocol)
USING(::apache::thrift::transport)

// Class that communicates with Leaderboard server
class LeaderboardManager : public Engine::CBase
{
	SINGLETON(LeaderboardManager)

private:
	boost::shared_ptr<TTransport>		m_transport;
	LeaderboardClient*					m_pClient;
	_bool								m_bConnected;

	std::map<_int, ranking_info>		m_mapData;

private:
	explicit LeaderboardManager();
	virtual ~LeaderboardManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready();

public:
	// Connect with Server
	void ConnectWithServer();
	// Disconnect with Server
	void DisconnectWithServer();
	// Set high score to the server
	void SetHighScore(_int playerId, _int score);
	// Get top 20 list from the server
	std::map<_int, ranking_info>* GetTop20();
	_bool GetConnected()		{ return m_bConnected; }
};

#endif //_LEADERBOARDMANAGER_H_