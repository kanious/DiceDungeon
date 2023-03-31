#include "pch.h"
#include "..\Headers\SQLiteDBHelper.h"


USING(Engine)
SINGLETON_FUNCTION(CSQLiteDBHelper)
USING(std)

int Callback(void*, int columns, char** text, char** names);

CSQLiteDBHelper::CSQLiteDBHelper()
	: m_pDB(nullptr), m_bIsConnected(false)
{
	m_mapData.clear();
}

CSQLiteDBHelper::~CSQLiteDBHelper()
{
}

// Call instead of destructor to manage class internal data
void CSQLiteDBHelper::Destroy()
{
	if (nullptr != m_pDB)
		sqlite3_close(m_pDB);
}

// Initialize
RESULT CSQLiteDBHelper::Ready(string filePath)
{
	ConnectToDB(filePath);

	return PK_NOERROR;
}

void CSQLiteDBHelper::ConnectToDB(string filePath)
{
	if (m_bIsConnected)
		return;

	_int result = sqlite3_open(filePath.c_str(), &m_pDB);
	if (SQLITE_OK != result)
		return;

	m_bIsConnected = true;
}

// Execute Query
void CSQLiteDBHelper::ExecuteQuery(const char* sql)
{
	if (!m_bIsConnected)
		return;

	m_mapData.clear();
	char* errMsg;
	_int result = sqlite3_exec(m_pDB, sql, Callback, &m_mapData, &errMsg);
	if (SQLITE_OK != result)
		printf("Query Execution failed. Error number is [%d:%s]\n", result, errMsg);
}

int Callback(void* mapContainer, int columns, char** text, char** names)
{
	map<_int, _int>& mapData = *(map<_int, _int>*)mapContainer;

	_int id = 0;
	_int score = 0;
	for (int i = 0; i < columns; ++i)
	{
		if (!strcmp("id", names[i]))
		{
			id = atoi(text[i]);
		}
		else
		{
			score = atoi(text[i]);
			mapData[id] = score;
		}
	}

	return 0;
}