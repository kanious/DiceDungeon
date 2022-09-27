#pragma once

#include "Defines.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CBase
{
protected:
	explicit CBase();
	virtual ~CBase();

private:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
};

NAMESPACE_END