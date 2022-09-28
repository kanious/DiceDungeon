#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include "Defines.h"
#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CComponent : public CBase
{
protected:
	eCOMPONENT			m_eType;
	_bool				m_bEnable;

protected:
	explicit CComponent();
	virtual ~CComponent();

protected:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();
	void Enable();
	void Disable();

public:
	const _bool GetEnable() { return m_bEnable; }
	const eCOMPONENT GetObjectType() { return m_eType; }
	void SetObjectType(eCOMPONENT);
};

NAMESPACE_END

#endif //_COMPONENT_H_