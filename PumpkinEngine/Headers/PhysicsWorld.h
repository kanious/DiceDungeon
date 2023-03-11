#ifndef _PHYSICSWORLD_H_
#define _PHYSICSWORLD_H_

#include <functional>
#include "iPhysicsWorld.h"
#include "CollisionHandler.h"
#include "CollisionData.h"
#include "Contact.h"
#include "ContactResolver.h"

NAMESPACE_BEGIN(Engine)

class CRigidBody;
class CCollisionHandler;
class CCamera;
class CComponent;

// Physics World class. This class controls all physics objects
class ENGINE_API CPhysicsWorld : public iPhysicsWorld
{
private:
	glm::vec3						m_vGravity;
	std::vector<CRigidBody*>		m_vecRigidBodies;
	CCollisionHandler*				m_pColHandler;
	CCamera*						m_pCamera;

	std::function<void(void)>		m_collisionCallback;

	_uint							m_iMaxContacts;
	CContact						m_contacts[256];
	CCollisionData					m_cData;
	CContactResolver				m_cResolver;

	_float							m_fElapsedTime;
	_float							m_fMaxTime;
	_bool							m_bRolling;

private:
	explicit CPhysicsWorld();
	virtual ~CPhysicsWorld();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	// Basic Update Function, update all rigid bodies
	virtual void Update(const _float& dt);

public:
	// Set gravity
	virtual void SetGravity(const glm::vec3& gravity);
	// Add rigid body and initailize collision box
	virtual void AddBody(iRigidBody* body);
	// Remove rigid body
	virtual void RemoveBody(iRigidBody* body);
	// Roll dice
	virtual void RollDice(_uint count);
	// Set camera object
	virtual void SetCamera(CComponent* pCamera);

private:
	// Initialize
	RESULT Ready(std::function<void(void)> callback);
public:
	// Create an instance
	static CPhysicsWorld* Create(std::function<void(void)> callback);
};

NAMESPACE_END

#endif //_PHYSICSWORLD_H_