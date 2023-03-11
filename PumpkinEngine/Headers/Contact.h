#ifndef _CONTACT_H_
#define _CONTACT_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"
#include <glm/glm.hpp>

NAMESPACE_BEGIN(Engine)

class CRigidBody;
// Contact information class
class ENGINE_API CContact
{
public:
	CRigidBody*		body[2];
	glm::vec3		contactPoint;
	glm::vec3		contactNormal;
	_float			friction;
	_float			restitution;
	_float			penetration;

	glm::mat3		contactToWorld = glm::mat3(1.f);
	glm::vec3		contactVelocity;
	_float			desiredDeltaVelocity;
	glm::vec3		relativeContactPosition[2];

public:
	explicit CContact();
	~CContact();
public:
	// Set rigidbody data
	void SetBodyData(CRigidBody* one, CRigidBody* two, _float friction, _float restitution);
	// Calculate relative contact position
	void CalculateInternals(const _float& dt);
	// Swap two rigidbodies
	void SwapBodies();
	// Awake body if it is sleeping
	void MatchAwakeState();
	// Calculate velocity to move
	void CalculateDesiredDeltaVelocity(const _float& dt);
	// Calculate local velocity
	glm::vec3 CalculateLocalVelocity(_uint bodyIndex, const _float& dt);
	//Calculate contact basis
	void CalculateContactBasis();
	// Apply velocity change to rigidbodies
	void ApplyVelocityChange(glm::vec3 velocityChange[2], glm::vec3 rotationChange[2]);
	// Apply posigion change to rigidbodies
	void ApplyPositionChange(glm::vec3 linearChange[2], glm::vec3 angularChange[2], _float penetration);
	// Calculate friction with less impulse
	glm::vec3 CalculateFrictionlessImpulse(glm::mat3* inverseInertiaTensor);
	// Calculate friction impulse
	glm::vec3 CalculateFrictionImpulse(glm::mat3* inverseInertiaTensor);
	// Set skew symmetric
	glm::mat3 SetSkewSymmetric(glm::mat3 m1, const glm::vec3 vector);
};

NAMESPACE_END

#endif //_CONTACT_H_