#include "pch.h"
#include "../Headers/PhysicsFactory.h"
#include "../Headers/PhysicsWorld.h"
#include "../Headers/RigidBody.h"
#include "../Headers/RigidBodyDesc.h"

USING(Engine)
USING(std)

CPhysicsFactory::CPhysicsFactory()
	: iPhysicsFactory()
{
}

CPhysicsFactory::~CPhysicsFactory()
{
}

// Call instead of destructor to manage class internal data
void CPhysicsFactory::Destroy()
{
}

// Create Physics World
iPhysicsWorld* CPhysicsFactory::CreateWorld(function<void(void)> callback)
{
	return CPhysicsWorld::Create(callback);
}

// Create RigidBody
iRigidBody* CPhysicsFactory::CreateRigidBody(const CRigidBodyDesc& desc, eShapeType shape)
{
	return CRigidBody::Create(desc, shape);
}

// Initialize
RESULT CPhysicsFactory::Ready()
{
	return PK_NOERROR;
}

// Create an instance
CPhysicsFactory* CPhysicsFactory::Create()
{
	CPhysicsFactory* pInstance = new CPhysicsFactory();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
