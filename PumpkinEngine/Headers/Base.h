#ifndef _BASE_H_
#define _BASE_H_

#include "EngineDefines.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CBase
{
private:
	_uint				m_iRefCnt;

protected:
	explicit CBase();
	virtual ~CBase();

public:
	_uint AddRefCnt();
	_uint Release();

protected:
	virtual void Destroy() = 0;
};

NAMESPACE_END

#endif //_BASE_H_