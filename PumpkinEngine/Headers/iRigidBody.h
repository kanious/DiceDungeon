#ifndef _IRIGIDBODY_H_
#define _IRIGIDBODY_H_

#include "Base.h"
#include "glm\vec3.hpp"
#include "glm\gtx\quaternion.hpp"

NAMESPACE_BEGIN(Engine)

class CTransform;

// Rigidbody interface class
class ENGINE_API iRigidBody : public CBase
{
protected:
	_uint			m_iValue;
	glm::mat4		m_matModel;
	_bool			m_bIsEnable;

protected:
	explicit iRigidBody() {}
	virtual ~iRigidBody() {}
	virtual void Destroy() = 0;

protected:
	CTransform* m_pParentTransform;
public:
	void SetTransform(CTransform* transform)	{ m_pParentTransform = transform; }
	void SetModelMatrix(glm::mat4 matModel)		{ m_matModel = matModel; }
	void SetEnable(_bool value)					{ m_bIsEnable = value; }
	glm::mat4 GetModelMatrix()					{ return m_matModel; }
	_bool GetEnable()							{ return m_bIsEnable; }

public:
	virtual glm::mat4 GetMatrixWorld() = 0;
};

NAMESPACE_END

#endif //_IRIGIDBODY_H_