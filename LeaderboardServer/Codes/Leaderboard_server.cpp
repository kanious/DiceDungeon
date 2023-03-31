#include "Leaderboard.h"
#include "leaderboard_types.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <atlconv.h>
#include <sstream>

#include "SQLiteDBHelper.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;

class LeaderboardHandler : virtual public LeaderboardIf
{
private:
	SQLiteDBHelper*			m_pSQLite;

public:
	LeaderboardHandler()
	{
		m_pSQLite = new SQLiteDBHelper();

		wchar_t path[MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, MAX_PATH);
		USES_CONVERSION;
		std::string str = W2A(path);
		str = str.substr(0, str.find_last_of("\\/"));
		std::stringstream ss;
		ss << str << "\\..\\Assets\\ranking.db";

		m_pSQLite->Ready(ss.str());
	}

	~LeaderboardHandler()
	{
		if (nullptr != m_pSQLite)
		{
			m_pSQLite->Destroy();
			delete m_pSQLite;
		}
	}

	void setHighScore(const int32_t playerId, const std::string& name, const int32_t highScore)
	{
		if (nullptr == m_pSQLite)
			return;

		std::stringstream ss;

		// Check id first
		ss << "SELECT * FROM DataTable WHERE id = '" << playerId << "';";
		m_pSQLite->ExecuteQuery(ss.str().c_str());
		if (0 < m_pSQLite->GetData()->size())
		{
			// Check if new score is higher than previous score
			std::map<int, ranking_info>::iterator iter = m_pSQLite->GetData()->begin();
			int prevScore = iter->second.highScore;
			if (prevScore < highScore)
			{
				// Update
				ss.str("");
				ss << "UPDATE DataTable SET name = '" << name << "', high_score = '" << highScore << "' ";
				ss << "WHERE id = '" << playerId << "';";
				m_pSQLite->ExecuteQuery(ss.str().c_str());
				printf("High score updated: (%d) %s - %d\n", playerId, name.c_str(), highScore);
			}
		}
		else
		{
			// Insert
			ss.str("");
			ss << "INSERT INTO DataTable VALUES(" << playerId << ", '" << name << "', " << highScore << ");";
			m_pSQLite->ExecuteQuery(ss.str().c_str());
			printf("High score added: (%d) %s - %d\n", playerId, name.c_str(), highScore);
		}
	}

	void getTop20(std::map<int32_t, ranking_info>& _return)
	{
		if (nullptr == m_pSQLite)
			return;

		std::stringstream ss;
		ss << "SELECT * FROM DataTable ORDER BY high_score DESC LIMIT 0, 20";
		m_pSQLite->ExecuteQuery(ss.str().c_str());

		std::map<int32_t, ranking_info>* pmap = m_pSQLite->GetData();

		_return.insert(pmap->begin(), pmap->end());

		printf("Top 20 list is collected\n");
	}
};

int main(int argc, char** argv)
{
	WSAData wsaData;
	int initializationResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initializationResult != 0)
	{
		printf("Failed to init winsock %d\n", initializationResult);
		return 1;
	}

	int port = 7890;
	shared_ptr<LeaderboardHandler> handler(new LeaderboardHandler());
	shared_ptr<TProcessor> processor(new LeaderboardProcessor(handler));
	shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
	shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
	shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());

	TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
	server.serve();

	return 0;
}

