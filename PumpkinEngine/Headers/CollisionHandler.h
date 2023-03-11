#ifndef _COLLISIONHANDLER_H_
#define _COLLISIONHANDLER_H_

#include "Base.h"
#include "glm\vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CRigidBody;
class CSphereShape;
class CPlaneShape;

// Collision Handling class
class CCollisionHandler : public CBase
{
public:
	struct sColPair
	{
		sColPair(CRigidBody* bodyA, CRigidBody* bodyB)
			: pBodyA(bodyA), pBodyB(bodyB)
		{}
		CRigidBody* pBodyA;
		CRigidBody* pBodyB;
	};

private:
	explicit CCollisionHandler();
	virtual ~CCollisionHandler();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	// Check for collisions between RigidBody
	void Collide(const _float& dt, std::vector<CRigidBody*>& bodies, std::vector<sColPair>& vecCols);

private: // Helper Functions
	// Sphere-Sphere Collision detection
	_bool CollideSphereSphere(const _float& dt, CRigidBody* bodyA, CSphereShape* sphereA,
		CRigidBody* bodyB, CSphereShape* sphereB);
	// Sphere-Plane Collision detection
	_bool CollideSpherePlane(const _float& dt, CRigidBody* sphereBody, CSphereShape* sphereShape,
		CRigidBody* planeBody, CPlaneShape* planeShape);
	// Sphere-Sphere Collision check
	_bool TestMovingSphereSphere(
		const glm::vec3& s0Center, const _float s0Radius,
		const glm::vec3& s1Center, const _float s1Radius,
		const glm::vec3& v0, const glm::vec3& v1, _float& t);
	// Sphere-Plane Collision check
	_bool TestMovingSpherePlane(
		const glm::vec3& prevPosition,
		const glm::vec3& curPosition,
		const glm::vec3& normal,
		_float radius, _float dotProduct);
	// Get closest point to plane
	glm::vec3 ClosestPtPointPlane(const glm::vec3& pt, const glm::vec3& planeNormal, _float planeDP);

private:
	// Initialize
	RESULT Ready();
public:
	// Create an instance
	static CCollisionHandler* Create();
};

NAMESPACE_END

#endif //_COLLISIONHANDLER_H_