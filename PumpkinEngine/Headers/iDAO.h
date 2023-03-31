#ifndef _IDAO_H_
#define _IDAO_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

// DAO interface class
class ENGINE_API iDAO : public CBase
{
protected:
	explicit iDAO() {}
	virtual ~iDAO() {}
	virtual void Destroy() = 0;

public:
	// Compare the new score and if it is higher than the last score, return true
	virtual _bool setHighScore(_int playerId, _int score) = 0;
	// Get the high score of the specific player id
	virtual _int getHighScore(_int playerId) = 0;

private:
	// Initialize
	virtual RESULT Ready(std::string filePath) = 0;
};

NAMESPACE_END

#endif //_IDAO_H_ 