#pragma once

#include "Defines.h"
#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CGameObject : CBase
{
protected:
	_float			m_viewDepthZ;
	_float			m_renderPriority;

	// Object Key나 enum 하나 파서 오브젝트 타입 구분해야 할 듯
	// 컴포넌트는 기본적인 건(Transform) 여기에 달고
	// 메쉬 등 객체 별로 달라야 하는 건 클라에서 추가하기


protected:
	explicit CGameObject();
	virtual ~CGameObject();

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