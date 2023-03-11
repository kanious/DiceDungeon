#include "pch.h"
#include "..\Headers\CollisionData.h"
#include "..\Headers\Contact.h"

USING(Engine)
USING(std)

CCollisionData::CCollisionData()
	: contactArray(nullptr), contacts(nullptr)
	, contactsLeft(0), contactCount(0), friction(0.f), restitution(0.f), tolerance(0.f)
{
}

CCollisionData::~CCollisionData()
{
}

// Return if there is contacts left
_bool CCollisionData::HasMoreContacts()
{
	return contactsLeft > 0;
}

// Reset collision data
void CCollisionData::Reset(_uint maxContacts)
{
	contactsLeft = maxContacts;
	contactCount = 0;
	contacts = contactArray;
}

// Add Contact data
void CCollisionData::AddContacts(_uint count)
{
	contactsLeft -= count;
	contactCount += count;
	contacts += count;
}
