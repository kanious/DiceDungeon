#ifndef _COLLISIONBOX_H_
#define _COLLISIONBOX_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"
#include "glm\mat4x4.hpp"
#include "iShape.h"

NAMESPACE_BEGIN(Engine)

class CRigidBody;
class IntersectionTests;
class CollisionDetector;

// Collision box for dice
class ENGINE_API CCollisionBox
{
public:
	static CCollisionBox*	boxData[256];
	static _uint			boxCount;

public:
	eShapeType				m_eShape;
	CRigidBody*				body;
	_uint					boxID;
	glm::mat4				offset = glm::mat4(1.0);
	glm::mat4				transform = glm::mat4(1.0);
	glm::mat4				modelMatrix = glm::mat4(1.0f);
	glm::vec3				halfSize;

	friend class IntersectionTests;
	friend class CollisionDetector;

public:
	explicit CCollisionBox(CRigidBody* pBody);
	~CCollisionBox();
public:
	// Returns the value of the axis corresponding to the index
	glm::vec3 GetAxis(_uint index) const;
	// Return transform
	const glm::mat4& GetTransform() const;
	// Return modelMatrix;
	glm::mat4 GetModelMatrix();
	// Set modelMatrix (to rigidbody also)
	void SetModelMatrix(glm::mat4 m);
	// Calculate transform
	void CalculateInternals();
};

NAMESPACE_END

#endif //_COLLISIONBOX_H_