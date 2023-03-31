#ifndef _SQLITEDBHELPER_H_
#define _SQLITEDBHELPER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API SQLiteDBHelper : public CBase
{
	SINGLETON(SQLiteDBHelper)

private:

private:
	explicit SQLiteDBHelper();
	~SQLiteDBHelper();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(std::string filePath);

private:
	// Execute Query
	void ExecuteQuery(const char* sql);
};

NAMESPACE_END

#endif //_SQLITEDBHELPER_H_