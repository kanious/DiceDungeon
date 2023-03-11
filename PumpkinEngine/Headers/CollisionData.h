#ifndef _COLLISIONDATA_H_
#define _COLLISIONDATA_H_

#include "EngineDefines.h"

NAMESPACE_BEGIN(Engine)

class CContact;
// Collision Data info class
class ENGINE_API CCollisionData
{
public:
	CContact*		contactArray;
	CContact*		contacts;
	_int			contactsLeft;
	_uint			contactCount;
	_float			friction;
	_float			restitution;
	_float			tolerance;

public:
	explicit CCollisionData();
	~CCollisionData();

public:
	// Return if there is contacts left
	_bool HasMoreContacts();
	// Reset collision data
	void Reset(_uint maxContacts);
	// Add Contact data
	void AddContacts(_uint count);
};

NAMESPACE_END

#endif //_COLLISIONDATA_H_