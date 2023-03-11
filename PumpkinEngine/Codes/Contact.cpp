#include "pch.h"
#include "..\Headers\Contact.h"
#include "..\Headers\RigidBody.h"
#include "..\Headers\OpenGLDefines.h"

USING(Engine)
USING(std)
USING(glm)

CContact::CContact()
{
}

CContact::~CContact()
{
}

// Set rigidbody data
void CContact::SetBodyData(CRigidBody* one, CRigidBody* two, _float friction, _float restitution)
{
	body[0] = one;
	body[1] = two;
	this->friction = friction;
	this->restitution = restitution;
}

// Calculate relative contact position
void CContact::CalculateInternals(const _float& dt)
{
    if (!body[0])
        SwapBodies();
    assert(body[0]);

    CalculateContactBasis();

    relativeContactPosition[0] = contactPoint - body[0]->position;
    if (body[1])
        relativeContactPosition[1] = contactPoint - body[1]->position;

    contactVelocity = CalculateLocalVelocity(0, dt);
    if (body[1])
        contactVelocity -= CalculateLocalVelocity(1, dt);

    CalculateDesiredDeltaVelocity(dt);
}

// Swap two rigidbodies
void CContact::SwapBodies()
{
    contactNormal *= -1.f;

    CRigidBody* temp = body[0];
    body[0] = body[1];
    body[1] = temp;
}

// Awake body if it is sleeping
void CContact::MatchAwakeState()
{
    if (!body[1])
        return;

    _bool body0awake = body[0]->isAwake;
    _bool body1awake = body[1]->isAwake;

    if (body0awake ^ body1awake)
    {
        if (body0awake)
            body[1]->isAwake = true;
        else
            body[0]->isAwake = true;
    }
}

// Calculate velocity to move
void CContact::CalculateDesiredDeltaVelocity(const _float& dt)
{
    const static _float velocityLimit = 0.25f;
    _float velocityFromAcc = 0;

    if (body[0]->isAwake)
        velocityFromAcc += dot(body[0]->lastFrameAcceleration * dt, contactNormal);
    if (body[1] && body[1]->isAwake)
        velocityFromAcc -=dot(body[1]->lastFrameAcceleration * dt, contactNormal);

    _float thisRestitution = restitution;
    if (fabs(contactVelocity.x) < velocityLimit)
        thisRestitution = 0.f;

    desiredDeltaVelocity = -contactVelocity.x - thisRestitution * (contactVelocity.x - velocityFromAcc);
}

// Calculate local velocity
vec3 CContact::CalculateLocalVelocity(_uint bodyIndex, const _float& dt)
{
    CRigidBody* thisBody = body[bodyIndex];

    vec3 velocity = cross(thisBody->rotation, relativeContactPosition[bodyIndex]);
    velocity += thisBody->velocity;

    vec3 contactVelocity = transpose(contactToWorld) * velocity;
    vec3 accVelocity = thisBody->lastFrameAcceleration * dt;
    accVelocity = transpose(contactToWorld) * accVelocity;
    accVelocity.x = 0;
    contactVelocity += accVelocity;

    return contactVelocity;
}

//Calculate contact basis
void CContact::CalculateContactBasis()
{
    vec3 contactTangent[2];

    if (fabs(contactNormal.x) > fabs(contactNormal.y))
    {
        const _float s = 1.f / sqrt(contactNormal.z * contactNormal.z + contactNormal.x * contactNormal.x);

        // The new X-axis is at right angles to the world Y-axis
        contactTangent[0].x = contactNormal.z * s;
        contactTangent[0].y = 0;
        contactTangent[0].z = -contactNormal.x * s;

        // The new Y-axis is at right angles to the new X- and Z- axes
        contactTangent[1].x = contactNormal.y * contactTangent[0].x;
        contactTangent[1].y = contactNormal.z * contactTangent[0].x -
                              contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = -contactNormal.y * contactTangent[0].x;
    }
    else
    {
        const _float s = 1.f / sqrt(contactNormal.z * contactNormal.z + contactNormal.y * contactNormal.y);

        // The new X-axis is at right angles to the world X-axis
        contactTangent[0].x = 0;
        contactTangent[0].y = -contactNormal.z * s;
        contactTangent[0].z = contactNormal.y * s;

        // The new Y-axis is at right angles to the new X- and Z- axes
        contactTangent[1].x = contactNormal.y * contactTangent[0].z -
                              contactNormal.z * contactTangent[0].y;
        contactTangent[1].y = -contactNormal.x * contactTangent[0].z;
        contactTangent[1].z = contactNormal.x * contactTangent[0].y;
    }

    contactToWorld = mat3(contactNormal, contactTangent[0], contactTangent[1]);
}

// Apply velocity change to rigidbodies
void CContact::ApplyVelocityChange(vec3 velocityChange[2], vec3 rotationChange[2])
{
    mat3 inverseInertiaTensor[2] = { mat3(1.0), mat3(1.0) };
    inverseInertiaTensor[0] = body[0]->inverseInertiaTensorWorld;
    if (body[1])
        inverseInertiaTensor[1] = body[1]->inverseInertiaTensorWorld;

    vec3 impulseContact;
    if (friction == 0.f)
        impulseContact = CalculateFrictionlessImpulse(inverseInertiaTensor);
    else
        impulseContact = CalculateFrictionImpulse(inverseInertiaTensor);

    vec3 impulse = impulseContact * transpose(contactToWorld);

    vec3 impulsiveTorque = cross(relativeContactPosition[0], impulse);
    rotationChange[0] = impulsiveTorque * inverseInertiaTensor[0];
    velocityChange[0] = vec3(0.f);
    velocityChange[0] += impulse * body[0]->inverseMass;

    body[0]->velocity += velocityChange[0];
    body[0]->rotation += rotationChange[0];

    if (body[1])
    {
        vec3 impulsiveTorque = cross(impulse, relativeContactPosition[1]);
        rotationChange[1] = impulsiveTorque * inverseInertiaTensor[1];
        velocityChange[1] = vec3(0.f);
        velocityChange[1] += impulse * -body[1]->inverseMass;

        body[1]->velocity += velocityChange[1];
        body[1]->rotation += rotationChange[1];
    }
}

// Apply posigion change to rigidbodies
void CContact::ApplyPositionChange(vec3 linearChange[2], vec3 angularChange[2], _float penetration)
{
    const _float angularLimit = 0.2f;
    _float angularMove[2];
    _float linearMove[2];

    _float totalInertia = 0.f;
    _float linearInertia[2];
    _float angularInertia[2];

    for (_uint i = 0; i < 2; i++) if (body[i])
    {
        mat3 inverseInertiaTensor = body[i]->inverseInertiaTensorWorld;

        vec3 angularInertiaWorld = cross(relativeContactPosition[i], contactNormal);
        angularInertiaWorld = angularInertiaWorld * inverseInertiaTensor;
        angularInertiaWorld = cross(angularInertiaWorld, relativeContactPosition[i]);
        angularInertia[i] = dot(angularInertiaWorld, contactNormal);

        linearInertia[i] = body[i]->inverseMass;
        totalInertia += linearInertia[i] + angularInertia[i];
    }

    for (unsigned i = 0; i < 2; i++) if (body[i])
    {
        _float sign = (i == 0.f) ? 1.f : -1.f;
        angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
        linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

        vec3 projection = relativeContactPosition[i];
        projection += contactNormal * -dot(relativeContactPosition[i], contactNormal);

        _float maxMagnitude = angularLimit * length(projection);
        if (angularMove[i] < -maxMagnitude)
        {
            _float totalMove = angularMove[i] + linearMove[i];
            angularMove[i] = -maxMagnitude;
            linearMove[i] = totalMove - angularMove[i];
        }
        else if (angularMove[i] > maxMagnitude)
        {
            _float totalMove = angularMove[i] + linearMove[i];
            angularMove[i] = maxMagnitude;
            linearMove[i] = totalMove - angularMove[i];
        }

        if (angularMove[i] == 0)
        {
            angularChange[i] = vec3(1.f);
        }
        else
        {
            vec3 targetAngularDirection = cross(relativeContactPosition[i], contactNormal);
            mat3 inverseInertiaTensor = body[i]->inverseInertiaTensorWorld;
            angularChange[i] =
                targetAngularDirection * inverseInertiaTensor * (angularMove[i] / angularInertia[i]);
        }

        linearChange[i] = contactNormal * linearMove[i];

        vec3 pos = body[i]->position;
        pos += contactNormal * linearMove[i];
        body[i]->position = pos;

        quat q = body[i]->orientation;
        q += quat(0, angularChange[i]) * q * 0.5f;
        body[i]->orientation = q;

        if (!body[i]->isAwake)
        {
            body[i]->orientation = normalize(body[i]->orientation);

            body[i]->transformMatrix = (mat4)body[i]->orientation;
            body[i]->transformMatrix[3] = vec4(body[i]->position, 1.f);
            body[i]->inverseInertiaTensorWorld = mat3(body[i]->transformMatrix) * body[i]->inverseInertiaTensor * mat3(inverse(body[i]->transformMatrix));
        };
    }
}

// Calculate friction with less impulse
vec3 CContact::CalculateFrictionlessImpulse(mat3* inverseInertiaTensor)
{
    vec3 impulseContact;

    vec3 deltaVelWorld = cross(relativeContactPosition[0], contactNormal);
    deltaVelWorld = deltaVelWorld * inverseInertiaTensor[0];
    deltaVelWorld = cross(deltaVelWorld, relativeContactPosition[0]);

    _float deltaVelocity = dot(deltaVelWorld, contactNormal);
    deltaVelocity += body[0]->inverseMass;
    if (body[1])
    {
        vec3 deltaVelWorld = cross(relativeContactPosition[1], contactNormal);
        deltaVelWorld = deltaVelWorld * inverseInertiaTensor[1];
        deltaVelWorld = cross(deltaVelWorld, relativeContactPosition[1]);

        deltaVelocity += dot(deltaVelWorld, contactNormal);
        deltaVelocity += body[1]->inverseMass;
    }

    impulseContact.x = desiredDeltaVelocity / deltaVelocity;
    impulseContact.y = 0.f;
    impulseContact.z = 0.f;

    return impulseContact;
}

// Calculate friction
vec3 CContact::CalculateFrictionImpulse(mat3* inverseInertiaTensor)
{
    vec3 impulseContact;
    _float inverseMass = body[0]->inverseMass;

    mat3 impulseToTorque = mat3(1.f);
    impulseToTorque = SetSkewSymmetric(impulseToTorque, relativeContactPosition[0]);

    mat3 deltaVelWorld = mat3(1.f);
    deltaVelWorld = impulseToTorque * inverseInertiaTensor[0] * impulseToTorque * -1.f;

    if (body[1])
    {
        impulseToTorque = SetSkewSymmetric(impulseToTorque, relativeContactPosition[1]);

        mat3 deltaVelWorld2 = mat3(1.f);
        deltaVelWorld2 = impulseToTorque * inverseInertiaTensor[1] * impulseToTorque * -1.f;
        deltaVelWorld += deltaVelWorld2;
        inverseMass += body[1]->inverseMass;
    }

    mat3 deltaVelocity = mat3(1.f);
    deltaVelocity = transpose(contactToWorld) * deltaVelWorld * contactToWorld;
    deltaVelocity[0][0] += inverseMass;
    deltaVelocity[1][1] += inverseMass;
    deltaVelocity[2][2] += inverseMass;

    mat3 impulseMatrix = inverse(deltaVelocity);

    vec3 velKill(desiredDeltaVelocity, -contactVelocity.y, -contactVelocity.z);

    impulseContact = velKill * impulseMatrix;

    _float planarImpulse = sqrt(impulseContact.y * impulseContact.y + impulseContact.z * impulseContact.z);

    if (planarImpulse > impulseContact.x * friction)
    {
        impulseContact.y /= planarImpulse;
        impulseContact.z /= planarImpulse;

        impulseContact.x = deltaVelocity[0][0] +
            deltaVelocity[0][1] * friction * impulseContact.y +
            deltaVelocity[0][2] * friction * impulseContact.z;

        impulseContact.x = desiredDeltaVelocity / impulseContact.x;
        impulseContact.y *= friction * impulseContact.x;
        impulseContact.z *= friction * impulseContact.x;
    }

    return impulseContact;
}

// Set skew symmetric
mat3 CContact::SetSkewSymmetric(mat3 m1, const vec3 vector)
{
    m1[0][0] = m1[1][1] = m1[2][2] = 0;
    m1[0][1] = -vector.z;
    m1[0][2] = vector.y;
    m1[1][0] = vector.z;
    m1[1][2] = -vector.x;
    m1[2][0] = -vector.y;
    m1[2][1] = vector.x;

    return m1;
}
