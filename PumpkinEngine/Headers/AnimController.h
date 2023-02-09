#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "Base.h"
#include "glm\mat4x4.hpp"

NAMESPACE_BEGIN(Engine)

class CAnimation;

class ENGINE_API CAnimController : public CBase
{
protected:
	CAnimation*				m_pCurAnimation;
	_uint					m_iFrameIndex;

	_float					m_fCurTime;
	_float					m_fAnimSpeed;
	_bool					m_bReverse;
	_bool					m_bPause;


protected:
	explicit CAnimController();
	virtual ~CAnimController();

protected:
	virtual void Destroy();
	virtual void AnimationEndEvent() = 0;

public:
	glm::mat4x4 GetMatrix();

public:
	void FrameMove(const _float& dt);
	void SetAnimationSpeed(_float speed) { m_fAnimSpeed = speed; }
	void SetNextFrame();
	void SetPreviousFrame();
	void SetReverse();
	void SetPause();
};

NAMESPACE_END

#endif //_ANIMCONTROLLER_H_