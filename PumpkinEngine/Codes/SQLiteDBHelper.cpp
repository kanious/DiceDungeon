#include "pch.h"
#include "..\Headers\SQLiteDBHelper.h"


USING(Engine)
SINGLETON_FUNCTION(SQLiteDBHelper)
USING(std)

SQLiteDBHelper::SQLiteDBHelper()
{
}

SQLiteDBHelper::~SQLiteDBHelper()
{
}

// Call instead of destructor to manage class internal data
void SQLiteDBHelper::Destroy()
{
}

// Initialize
RESULT SQLiteDBHelper::Ready(string filePath)
{

	return PK_NOERROR;
}

// Execute Query
void SQLiteDBHelper::ExecuteQuery(const char* sql)
{
}
