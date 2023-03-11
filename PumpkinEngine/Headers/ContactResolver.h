#ifndef _CONTACTRESOLVER_H_
#define _CONTACTRESOLVER_H_

#include "EngineDefines.h"

NAMESPACE_BEGIN(Engine)

class CContact;
// Contact resolving class
class ENGINE_API CContactResolver
{
public:
	_uint		velocityIterations;
	_uint		positionIterations;
	_float		velocityEpsilon;
	_float		positionEpsilon;
	_uint		velocityIterationsUsed;
	_uint		positionIterationsUsed;
private:
	_bool		validSettings;

public:
	explicit CContactResolver();
	explicit CContactResolver(_uint iterations, _float velocityEpsilon = 0.01f, _float positionEpsilon = 0.01f);
	~CContactResolver();

public:
	// Resolve contact after collision
	void ResolveContacts(CContact* contactArray, _uint numContacts, const _float& dt);
	// Check status
	_bool isValid();
	// Set iterations
	void SetIterations(_uint velocityInterations, _uint positionIterations);
	// Set Epsilons
	void SetEpsilon(_float velocityEpsilon, _float positionEpsilon);

private:
	// Prepare contacts
	void PrepareContacts(CContact* contactArray, _uint numContacts, _float dt);
	// Adjust velocity to contacts
	void AdjustVelocities(CContact* contactArray, _uint numContacts, _float dt);
	// Adjust position to contacts
	void AdjustPositions(CContact* contactArray, _uint numContacts, _float dt);
};

NAMESPACE_END

#endif //_CONTACTRESOLVER_H_