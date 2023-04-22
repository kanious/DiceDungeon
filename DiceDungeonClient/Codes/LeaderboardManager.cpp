#include "LeaderboardManager.h"
#include <sstream>


SINGLETON_FUNCTION(LeaderboardManager)
USING(Engine)
USING(std)

LeaderboardManager::LeaderboardManager()
	: m_bConnected(false)
{
	m_mapData.clear();
}

LeaderboardManager::~LeaderboardManager()
{
}

// Call instead of destructor to manage class internal data
void LeaderboardManager::Destroy()
{
	if (nullptr != m_pClient)
		delete m_pClient;

	if (m_bConnected)
		m_transport->close();

	m_mapData.clear();
}

// Initialize
RESULT LeaderboardManager::Ready()
{
	//ConnectWithServer();

	return PK_NOERROR;
}

// Connect with Server
void LeaderboardManager::ConnectWithServer()
{
	boost::shared_ptr<TSocket> socket(new TSocket("localhost", 7890));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	m_transport = transport;
	m_pClient = new LeaderboardClient(protocol);

	try
	{
		m_transport->open();
	}
	catch (TTransportException e)
	{
		cout << "Server connect failed..." << endl;
		m_bConnected = false;
		return;
	}

	m_bConnected = true;
}

void LeaderboardManager::DisconnectWithServer()
{
	if (nullptr != m_pClient)
		delete m_pClient;

	if (m_bConnected)
		m_transport->close();

	m_mapData.clear();

	m_bConnected = false;
}

// Set high score to the server
void LeaderboardManager::SetHighScore(_int playerId, _int score)
{
	if (!m_bConnected)
		return;

	stringstream ss;
	ss << "Player_" << playerId;

	m_pClient->setHighScore(playerId, ss.str(), score);
}

// Get top 20 list from the server
map<_int, ranking_info>* LeaderboardManager::GetTop20()
{
	if (!m_bConnected)
		return nullptr;

	m_mapData.clear();

	m_pClient->getTop20(m_mapData);

	return &m_mapData;
}
