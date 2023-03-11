#ifndef _PHYSICSFACTORY_H_
#define _PHYSICSFACTORY_H_

#include "iPhysicsFactory.h"

NAMESPACE_BEGIN(Engine)

// Physics Factory class
class ENGINE_API CPhysicsFactory : public iPhysicsFactory
{
private:
	explicit CPhysicsFactory();
	virtual ~CPhysicsFactory();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	// Create Physics World
	virtual iPhysicsWorld* CreateWorld(std::function<void(void)> callback);
	// Create RigidBody
	virtual iRigidBody* CreateRigidBody(const CRigidBodyDesc& desc, eShapeType shape);

private:
	// Initialize
	RESULT Ready();
public:
	// Create an instance
	static CPhysicsFactory* Create();
};

NAMESPACE_END

#endif //_PHYSICSFACTORY_H_