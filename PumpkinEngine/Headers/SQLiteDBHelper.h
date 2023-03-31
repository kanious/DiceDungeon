#ifndef _SQLITEDBHELPER_H_
#define _SQLITEDBHELPER_H_

#include "Base.h"
#include "..\sqlite\sqlite3.h"
#include <map>

NAMESPACE_BEGIN(Engine)

class ENGINE_API CSQLiteDBHelper : public CBase
{
	SINGLETON(CSQLiteDBHelper)

private:
	sqlite3*		m_pDB;
	_bool			m_bIsConnected;

	std::map<_int, _int> m_mapData;

private:
	explicit CSQLiteDBHelper();
	~CSQLiteDBHelper();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(std::string filePath);

private:
	// Connect to DB file
	void ConnectToDB(std::string filePath);
public:
	// Execute Query
	void ExecuteQuery(const char* sql);
	std::map<_int, _int>* GetData()		{ return &m_mapData; }
};

NAMESPACE_END

#endif //_SQLITEDBHELPER_H_