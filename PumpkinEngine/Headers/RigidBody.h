#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "EngineDefines.h"
#include "OpenGLDefines.h"
#include "iRigidBody.h"
#include "iShape.h"

NAMESPACE_BEGIN(Engine)

class CRigidBodyDesc;
// Rigid body class
class ENGINE_API CRigidBody : public iRigidBody
{
public:
	eShapeType eShape;
	_float inverseMass;

	glm::mat3 inverseInertiaTensor = glm::mat3(1.0);
	glm::mat3 inverseInertiaTensorWorld = glm::mat3(1.0);

	_float linearDamping;
	_float angularDamping;
	_float forceDamping;

	glm::vec3 position;
	glm::vec3 velocity;
	glm::quat orientation;
	glm::vec3 halfSize;
	glm::vec3 rotation;
	glm::vec3 gravity;
	glm::vec3 lastFrameAcceleration;
	glm::mat4 transformMatrix = glm::mat4(1.0);

	_float motion;
	_bool isAwake;
	_bool canSleep;
	_bool isFinished;

	glm::vec3 forceAccum;
	glm::vec3 torqueAccum;

	_float gap = 2.f;

private:
	explicit CRigidBody();
	virtual ~CRigidBody();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	virtual glm::mat4 GetMatrixWorld()			{ return transformMatrix; }
	void SetGravity(glm::vec3 gravity)			{ this->gravity = gravity; }
public:
	// Basic Update Function
	void Update(const _float& dt);
	// Integrate all state
	void Integrate(const _float& dt);
	// Kill forces
	void KillForces();
	// Reset rigid body state
	void ResetRigidBody(const CRigidBodyDesc& desc);
private:
	// Set inertia tensor for dice roll
	glm::mat3 SetBlockInertiaTensor(glm::mat3 m, const glm::vec3& halfSizes, _float mass);
	// Set inertial tensor coeffs for dice roll
	glm::mat3 SetInertialTensorCoeffs(glm::mat3 m, _float ix, _float iy, _float iz,
		_float ixy = 0, _float ixz = 0, _float iyz = 0);
	// Set mass
	void SetMass(const _float mass);
	// Set awake
	void SetAwake(const _bool awake = true);
	// Set can sleep
	void SetCanSleep(const _bool canSleep = true);
	// Check dir to confirm dice value
	void CheckDir();

private:
	// Initialize
	RESULT Ready(const CRigidBodyDesc& desc, eShapeType shape);
public:
	// Create an instance
	static CRigidBody* Create(const CRigidBodyDesc& desc, eShapeType shape);
};

NAMESPACE_END

#endif //_RIGIDBODY_H_