#ifndef _COLLISIONDETECTOR_H_
#define _COLLISIONDETECTOR_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CCollisionBox;
class CCollisionData;

// The Class to detect collision between two rigidbodies
class ENGINE_API CCollisionDetector
{
public:
	// Check if two objects collide and add Contact
	static _uint BoxAndBox(const CCollisionBox& one, const CCollisionBox& two, CCollisionData* data);

	// Check Axis
	static inline _bool TryAxis(const CCollisionBox& one, const CCollisionBox& two, glm::vec3 axis,
		const glm::vec3& toCentre, _uint index, _float& smallestPenetration, _uint& smallestCase);

	// Check if two objects penetrate each other
	static _float PenetrationOnAxis(const CCollisionBox& one, const CCollisionBox& two, 
		const glm::vec3& axis, const glm::vec3& toCentre);

	// Return project value
	static inline _float TransformToAxis(const CCollisionBox& box, const glm::vec3& axis);

	// Fill point
	static void FillPointFaceBoxBox(const CCollisionBox& one, const CCollisionBox& two,
		const glm::vec3& toCentre, CCollisionData* data, _uint best, _float pen);

	// Return contact point between two objects
	static inline glm::vec3 ContactPoint(const glm::vec3& pOne, const glm::vec3& dOne, _float oneSize,
		const glm::vec3& pTwo, const glm::vec3& dTwo, _float twoSize, _bool useOne);
};

NAMESPACE_END

#endif //_COLLISIONDETECTOR_H_