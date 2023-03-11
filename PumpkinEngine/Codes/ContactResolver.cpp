#include "pch.h"
#include "..\Headers\ContactResolver.h"
#include "..\Headers\Contact.h"
#include "..\Headers\OpenGLDefines.h"

USING(Engine)
USING(std)
USING(glm)

CContactResolver::CContactResolver()
{
}

CContactResolver::CContactResolver(_uint iterations, _float velocityEpsilon, _float positionEpsilon)
{
	SetIterations(iterations, iterations);
	SetEpsilon(velocityEpsilon, positionEpsilon);
}

CContactResolver::~CContactResolver()
{
}

// Resolve contact after collision
void CContactResolver::ResolveContacts(CContact* contactArray, _uint numContacts, const _float& dt)
{
	if (0 == numContacts)
		return;

	if (!isValid())
		return;

	PrepareContacts(contactArray, numContacts, dt);
	AdjustVelocities(contactArray, numContacts, dt);
	AdjustPositions(contactArray, numContacts, dt);
}

// Check status
_bool CContactResolver::isValid()
{
	return (velocityIterations > 0) && (positionIterations > 0) &&
		(velocityEpsilon >= 0.0f) && (positionEpsilon >= 0.0f);
}

// Set iterations
void CContactResolver::SetIterations(_uint velocityInterations, _uint positionIterations)
{
	this->velocityIterations = velocityInterations;
	this->positionIterations = positionIterations;
}

// Set Epsilons
void CContactResolver::SetEpsilon(_float velocityEpsilon, _float positionEpsilon)
{
	this->velocityEpsilon = velocityEpsilon;
	this->positionEpsilon = positionEpsilon;
}

// Prepare contacts
void CContactResolver::PrepareContacts(CContact* contactArray, _uint numContacts, _float dt)
{
	CContact* lastContact = contactArray + numContacts;
	for (CContact* contact = contactArray; contact < lastContact; ++contact)
		contact->CalculateInternals(dt);
}

// Adjust velocity to contacts
void CContactResolver::AdjustVelocities(CContact* contactArray, _uint numContacts, _float dt)
{
    vec3 velocityChange[2], rotationChange[2];
    vec3 deltaVel;

    velocityIterationsUsed = 0;
    while (velocityIterationsUsed < velocityIterations)
    {
        _float max = velocityEpsilon;
        _uint index = numContacts;
        for (_uint i = 0; i < numContacts; i++)
        {
            if (contactArray[i].desiredDeltaVelocity > max)
            {
                max = contactArray[i].desiredDeltaVelocity;
                index = i;
            }
        }

        if (index == numContacts)
            break;

        contactArray[index].MatchAwakeState();
        contactArray[index].ApplyVelocityChange(velocityChange, rotationChange);

        for (_uint i = 0; i < numContacts; i++)
        {
            for (_uint b = 0; b < 2; b++) if (contactArray[i].body[b])
            {
                for (_uint d = 0; d < 2; d++)
                {
                    if (contactArray[i].body[b] == contactArray[index].body[d])
                    {
                        deltaVel = velocityChange[d] +
                            cross(rotationChange[d], contactArray[i].relativeContactPosition[b]);

                        contactArray[i].contactVelocity +=
                            transpose(contactArray[i].contactToWorld) * deltaVel * (b ? -1.f : 1.f);
                        contactArray[i].CalculateDesiredDeltaVelocity(dt);
                    }
                }
            }
        }
        velocityIterationsUsed++;
    }
}

// Adjust position to contacts
void CContactResolver::AdjustPositions(CContact* contactArray, _uint numContacts, _float dt)
{
    _uint i, index;
    vec3 linearChange[2], angularChange[2];
    _float max;
    vec3 deltaPosition;

    positionIterationsUsed = 0;
    while (positionIterationsUsed < positionIterations)
    {
        max = positionEpsilon;
        index = numContacts;
        for (i = 0; i < numContacts; i++)
        {
            if (contactArray[i].penetration > max)
            {
                max = contactArray[i].penetration;
                index = i;
            }
        }
        if (index == numContacts) break;

        contactArray[index].MatchAwakeState();
        contactArray[index].ApplyPositionChange(linearChange, angularChange, max);

        for (i = 0; i < numContacts; i++)
        {
            for (unsigned b = 0; b < 2; b++) if (contactArray[i].body[b])
            {
                for (unsigned d = 0; d < 2; d++)
                {
                    if (contactArray[i].body[b] == contactArray[index].body[d])
                    {
                        deltaPosition = linearChange[d] +
                            cross(angularChange[d], contactArray[i].relativeContactPosition[b]);
                        contactArray[i].penetration +=
                            dot(deltaPosition, contactArray[i].contactNormal) * (b ? 1.f : -1.f);
                    }
                }
            }
        }
        positionIterationsUsed++;
    }
}
