#include "pch.h"
#include "../Headers/DAO.h"
#include "../Headers/SQLiteDBHelper.h"
#include <sstream>

USING(Engine)
USING(std)

CDAO::CDAO()
{
	m_pSQLite = CSQLiteDBHelper::GetInstance(); m_pSQLite->AddRefCnt();
}

CDAO::~CDAO()
{
}

// Call instead of destructor to manage class internal data
void CDAO::Destroy()
{
	SafeDestroy(m_pSQLite);
}

// Compare the new score and if it is higher than the last score, return true
_bool CDAO::setHighScore(_int playerId, _int score)
{
	stringstream ss;

	// Check id first
	ss << "SELECT * FROM DataTable WHERE id = '" << playerId << "';";
	m_pSQLite->ExecuteQuery(ss.str().c_str());
	if (0 < m_pSQLite->GetData()->size())
	{
		// Check if new score is higher than previous score
		map<_int, _int>::iterator iter = m_pSQLite->GetData()->begin();
		_int prevScore = iter->second;
		if (prevScore < score)
		{
			// Update
			ss.str("");
			ss << "UPDATE DataTable SET high_score = '" << score << "' WHERE id = '" << playerId << "';";
			m_pSQLite->ExecuteQuery(ss.str().c_str());
			return true;
		}
		else
		{
			// Do nothing
			return false;
		}
	}
	else
	{
		// Insert
		ss.str("");
		ss << "INSERT INTO DataTable VALUES(" << playerId << ", " << score << ");";
		m_pSQLite->ExecuteQuery(ss.str().c_str());

		return true;
	}
}
 
// Get the high score of the specific player id
_int CDAO::getHighScore(_int playerId)
{
	stringstream ss;
	ss << "SELECT * FROM DataTable WHERE id = '" << playerId << "';";
	m_pSQLite->ExecuteQuery(ss.str().c_str());

	if (0 < m_pSQLite->GetData()->size())
	{
		map<_int, _int>::iterator iter = m_pSQLite->GetData()->begin();
		return iter->second;
	}

	return -1;
}

// Initialize
RESULT CDAO::Ready(string filePath)
{
	m_pSQLite->Ready(filePath);

	return PK_NOERROR;
}

// Create an instance
CDAO* CDAO::Create(string filePath)
{
	CDAO* pInstance = new CDAO();
	if (PK_NOERROR != pInstance->Ready(filePath))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
