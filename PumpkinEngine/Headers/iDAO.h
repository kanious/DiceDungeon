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
	virtual void setHighScore(_int playerId, _int score) = 0;
	virtual _int getHighScore(_int playerId) = 0;
};

NAMESPACE_END

#endif //_IDAO_H_ 