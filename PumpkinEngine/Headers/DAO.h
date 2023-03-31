#ifndef _DAO_H_
#define _DAO_H_

#include "iDAO.h"

NAMESPACE_BEGIN(Engine)

// DAO class
class ENGINE_API CDAO : public iDAO
{
private:
	explicit CDAO();
	virtual ~CDAO();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	virtual void setHighScore(_int playerId, _int score);
	virtual _int getHighScore(_int playerId);

private:
	// Initialize
	RESULT Ready();
public:
	// Create an instance
	static CDAO* Create();
};

NAMESPACE_END

#endif //_PHYSICSWORLD_H_