#include "pch.h"
#include "../Headers/RigidBody.h"
#include "../Headers/RigidBodyDesc.h"
#include "../Headers/Transform.h"
#include "../Headers/DiceMaster.h"

USING(Engine)
USING(std)
USING(glm)

_float safe_acos(_float value)
{
	if (value <= -1.0f)
		return radians(180.f);
	else if (value >= 1.0f)
		return 0.f;
	else
		return acos(value);
}
_float GetAngle(vec3 vDir1, vec3 vDir2)
{
	_float fDot = dot(vDir1, vDir2);
	_float fAngleGap = safe_acos(fDot);
	return degrees(fAngleGap);
}

CRigidBody::CRigidBody()
{
	m_iValue = 0;
}

CRigidBody::~CRigidBody()
{
}

// Call instead of destructor to manage class internal data
void CRigidBody::Destroy()
{
}

// Basic Update Function
void CRigidBody::Update(const _float& dt)
{
}

// Integrate all state
void CRigidBody::Integrate(const _float& dt)
{
	if (isFinished)
		return;

	if (!isAwake)
		return;

	lastFrameAcceleration = gravity;
	lastFrameAcceleration += forceAccum * inverseMass;

	vec3 angularAcceleration = torqueAccum * inverseInertiaTensorWorld;

	velocity += lastFrameAcceleration * dt;
	rotation += angularAcceleration * dt;

	velocity *= pow(linearDamping, dt);
	rotation *= pow(angularDamping, dt);

	position += velocity * dt;
	orientation += quat(0, rotation * dt) * orientation * 0.5f;
	orientation = normalize(orientation);

	transformMatrix = (mat4)orientation;
	transformMatrix[3] = vec4(position, 1.f);

	inverseInertiaTensorWorld = mat3(transformMatrix) * inverseInertiaTensor * mat3(inverse(transformMatrix));

	CheckDir();

	if (canSleep)
	{
		_float currentMotion = dot(velocity, velocity) + dot(rotation, rotation);
		_float bias = pow(0.5, dt);
		motion = bias * motion + (1.f - bias) * currentMotion;

		if (motion < 0.3f)
			SetAwake(false);
		else if (motion > 10.f * 0.3f)
			motion = 10.f * 0.3f;
	}
}

// Kill forces
void CRigidBody::KillForces()
{
	forceAccum *= forceDamping;
	torqueAccum = vec3(0.0, 0.0, 0.0);
}

// Reset rigid body state
void CRigidBody::ResetRigidBody(const CRigidBodyDesc& desc)
{
	Ready(desc, eShape);
}

// Set inertia tensor for dice roll
mat3 CRigidBody::SetBlockInertiaTensor(mat3 m, const vec3& halfSizes, _float mass)
{
	vec3 squares = halfSizes * halfSizes;
	m = SetInertialTensorCoeffs(m,
		0.3f * mass * (squares.y + squares.z),
		0.3f * mass * (squares.x + squares.z),
		0.3f * mass * (squares.x + squares.y));
	return m;
}

// Set inertial tensor coeffs for dice roll
mat3 CRigidBody::SetInertialTensorCoeffs(mat3 m, _float ix, _float iy, _float iz, _float ixy, _float ixz, _float iyz)
{
	m[0][0] = ix;
	m[0][1] = m[1][0] = -ixy;
	m[0][2] = m[2][0] = -ixz;
	m[1][1] = iy;
	m[1][2] = m[2][1] = -iyz;
	m[2][2] = iz;

	return m;
}

// Set mass
void CRigidBody::SetMass(const _float mass)
{
	assert(mass != 0.f);
	inverseMass = 1.f / mass;
}

// Set awake
void CRigidBody::SetAwake(const _bool awake)
{
	if (awake)
	{
		isAwake = true;
		motion = 0.3f * 2.f;
	}
	else
	{
		isAwake = false;
		velocity = vec3(0.f);
		rotation = vec3(0.f);
	}
}

// Set can sleep
void CRigidBody::SetCanSleep(const _bool canSleep)
{
	this->canSleep = canSleep;

	if (!canSleep && !isAwake)
		SetAwake();
}

// Check dir to confirm dice value
void CRigidBody::CheckDir()
{
	if (position.y > 5.f)
		return;

	vec3 yplus = vec3(0.f, 1.f, 0.f);

	vec3 vUp = transformMatrix[1];
	vUp = normalize(vUp);
	
	_float angleGap = GetAngle(vUp, yplus);
	if (angleGap <= gap)
	{
		isFinished = true;
		m_iValue = 3;
	}
	else if (angleGap > 180.f - gap)
	{
		isFinished = true;
		m_iValue = 4;
	}
	else
	{
		vec3 vRight = transformMatrix[0];
		vRight = normalize(vRight);
		float angleGap2 = GetAngle(vRight, yplus);
		if (angleGap2 <= gap)
		{
			isFinished = true;
			m_iValue = 5;
		}
		else if (angleGap2 > 180.f - gap)
		{
			isFinished = true;
			m_iValue = 2;
		}
		else
		{
			vec3 vLook = transformMatrix[2];
			vLook = normalize(vLook);
			float angleGap3 = GetAngle(vLook, yplus);
			if (angleGap3 <= gap)
			{
				isFinished = true;
				m_iValue = 1;
			}
			else if (angleGap3 > 180.f - gap)
			{
				isFinished = true;
				m_iValue = 6;
			}
		}
	}

	if (isFinished)
		CDiceMaster::GetInstance()->AddAP(m_iValue);
}

// Initialize
RESULT CRigidBody::Ready(const CRigidBodyDesc& desc, eShapeType shape)
{
	eShape = shape;

	position = vec3(desc.position.x, desc.position.y, desc.position.z);
	orientation = quat(desc.orientation);
	velocity = vec3(0.f);
	rotation = vec3(desc.rotation.x, desc.rotation.y, desc.rotation.z);
	
	halfSize = vec3(desc.halfSize.x, desc.halfSize.y, desc.halfSize.z);
	_float mass = halfSize.x * halfSize.y * halfSize.z * 8.f;
	SetMass(mass);

	mat3 tensor = mat3(1.f);
	tensor = SetBlockInertiaTensor(tensor, halfSize, mass);
	inverseInertiaTensor = inverse(tensor);
	linearDamping = 0.95f;
	angularDamping = 0.8f;
	forceDamping = 0.8f;
	forceAccum = vec3(desc.forceAccum.x, desc.forceAccum.y, desc.forceAccum.z);
	torqueAccum = vec3(0.f);
	gravity = vec3(0.f);

	SetCanSleep(false);
	if (eShapeType::Plane == shape)
		isFinished = true;
	else
		isFinished = false;
	isAwake = true;
	m_bIsEnable = true;
	m_iValue = 0;

	orientation = normalize(orientation);
	
	orientation += quat(0, rotation) * orientation * 0.5f;
	orientation = normalize(orientation);

	transformMatrix = (mat4)orientation;
	transformMatrix[3] = vec4(position, 1.0);

	mat3 a = mat3(transformMatrix);
	mat3 b = mat3(inverse(transformMatrix));

	inverseInertiaTensorWorld = mat3(transformMatrix) * inverseInertiaTensor * mat3(inverse(transformMatrix));

	return PK_NOERROR;
}

// Create an instance
CRigidBody* CRigidBody::Create(const CRigidBodyDesc& desc, eShapeType shape)
{
	CRigidBody* pInstance = new CRigidBody();
	if (PK_NOERROR != pInstance->Ready(desc, shape))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
