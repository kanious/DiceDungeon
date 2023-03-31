#ifndef _DAO_H_
#define _DAO_H_

#include "iDAO.h"

NAMESPACE_BEGIN(Engine)

class CSQLiteDBHelper;

// DAO class
class ENGINE_API CDAO : public iDAO
{
private:
	CSQLiteDBHelper*		m_pSQLite;

private:
	explicit CDAO();
	virtual ~CDAO();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	// Compare the new score and if it is higher than the last score, return true
	virtual _bool setHighScore(_int playerId, _int score);
	// Get the high score of the specific player id
	virtual _int getHighScore(_int playerId);

private:
	// Initialize
	virtual RESULT Ready(std::string filePath);
public:
	// Create an instance
	static CDAO* Create(std::string filePath);
};

NAMESPACE_END

#endif //_PHYSICSWORLD_H_