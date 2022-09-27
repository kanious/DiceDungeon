#pragma once

#include "Defines.h"
#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CComponent :CBase
{
protected:
	explicit CComponent();
	virtual ~CComponent();

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