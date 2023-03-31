#ifndef _SQLITEDBHELPER_H_
#define _SQLITEDBHELPER_H_

#include "..\sqlite\sqlite3.h"
#include <map>
#include <string>
#include "leaderboard_types.h"

class SQLiteDBHelper
{
private:
	sqlite3*						m_pDB;
	bool							m_bIsConnected;

	std::map<int, ranking_info>		m_mapData;

public:
	explicit SQLiteDBHelper();
	~SQLiteDBHelper();
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	void Ready(std::string filePath);

private:
	// Connect to DB file
	void ConnectToDB(std::string filePath);
public:
	// Execute Query
	void ExecuteQuery(const char* sql);
	std::map<int, ranking_info>* GetData()		{ return &m_mapData; }
};

#endif //_SQLITEDBHELPER_H_