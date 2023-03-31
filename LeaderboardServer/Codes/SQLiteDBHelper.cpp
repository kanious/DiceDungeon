#include "SQLiteDBHelper.h"

using namespace std;

int Callback(void*, int columns, char** text, char** names);

SQLiteDBHelper::SQLiteDBHelper()
	: m_pDB(nullptr), m_bIsConnected(false)
{
	m_mapData.clear();
}

SQLiteDBHelper::~SQLiteDBHelper()
{
}

// Call instead of destructor to manage class internal data
void SQLiteDBHelper::Destroy()
{
	if (nullptr != m_pDB)
		sqlite3_close(m_pDB);
}

// Initialize
void SQLiteDBHelper::Ready(string filePath)
{
	ConnectToDB(filePath);
}

void SQLiteDBHelper::ConnectToDB(string filePath)
{
	if (m_bIsConnected)
		return;

	int result = sqlite3_open(filePath.c_str(), &m_pDB);
	if (SQLITE_OK != result)
		return;

	m_bIsConnected = true;
}

// Execute Query
void SQLiteDBHelper::ExecuteQuery(const char* sql)
{
	if (!m_bIsConnected)
		return;

	m_mapData.clear();
	char* errMsg;
	int result = sqlite3_exec(m_pDB, sql, Callback, &m_mapData, &errMsg);
	if (SQLITE_OK != result)
		printf("Query Execution failed. Error number is [%d:%s]\n", result, errMsg);
}

int Callback(void* mapContainer, int columns, char** text, char** names)
{
	map<int, ranking_info>& mapData = *(map<int, ranking_info>*)mapContainer;

	int id = 0;
	string name = "";
	int score = 0;
	for (int i = 0; i < columns; ++i)
	{
		if (!strcmp("id", names[i]))
		{
			id = atoi(text[i]);
		}
		else if (!strcmp("name", names[i]))
		{
			name = text[i];
		}
		else
		{
			score = atoi(text[i]);

			ranking_info newInfo;
			newInfo.playerId = id;
			newInfo.name = name;
			newInfo.highScore = score;
			mapData[id] = newInfo;
		}
	}

	return 0;
}