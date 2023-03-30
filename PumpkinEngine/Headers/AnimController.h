#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "Base.h"
#include "glm\mat4x4.hpp"

NAMESPACE_BEGIN(Engine)

class CAnimation;
 
//A class that has data for actually playing animations. Each object that want to use animation for has each one.
class ENGINE_API CAnimController : public CBase
{
protected:
	CAnimation*					m_pCurAnimation;
	_uint						m_iFrameIndex;
	std::string					m_curTag;

	_float						m_fCurTime;
	_float						m_fAnimSpeed;
	_bool						m_bReverse;
	_bool						m_bPause;
	_bool						m_bIsPlaying;

	// Blending
	_bool						m_bChangeAnimation;
	std::vector<glm::mat4x4>*	m_pvecPrevTransforms;
	std::string					m_prevTag;
	_float						m_fWeight;
	_float						m_fLastAccumWeight;
	_float						m_fTotalAccumWeight;
	_uint						m_iPrevFrameIndex;

protected:
	explicit CAnimController();
	virtual ~CAnimController();

protected:
	virtual void Destroy();
	virtual void AnimationEndEvent() = 0;

public:
	std::vector<glm::mat4x4>* GetMatrix();
	std::vector<glm::mat4x4>* GetPrevMatrix();
	_bool GetIsPlaying()				{ return m_bIsPlaying; }
	_bool GetBlending()					{ return m_bChangeAnimation; }
	_float GetBlendingFactor()			{ return m_fTotalAccumWeight; }
	void SetIsPlaying(_bool value)		{ m_bIsPlaying = value; }

public:
	void FrameMove(const _float& dt);
	void SetAnimationSpeed(_float speed) { m_fAnimSpeed = speed; }
	void SetNextFrame();
	void SetPreviousFrame();
	void SetReverse();
	void SetPause();
	void StopAnimation();
	void ResetAnimation();
public:
	void InitBlendingVariables();
	void StartBlending();
	void BlendingMove();
};

NAMESPACE_END

#endif //_ANIMCONTROLLER_H_