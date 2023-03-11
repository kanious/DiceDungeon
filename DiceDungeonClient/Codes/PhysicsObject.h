#ifndef _PHYSICSOBJECT_H_
#define _PHYSICSOBJECT_H_

#include "GameObject.h"
#include "EngineStruct.h"

namespace Engine
{
	class CMesh;
	class iRigidBody;
}

// Physics Object class (for dice)
class PhysicsObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	Engine::iRigidBody*				m_pRigidBody;

private:
	explicit PhysicsObject();
	virtual ~PhysicsObject();

public:
	// Set rigidbody for this object
	void SetRigidBody(Engine::iRigidBody* pBody)	{ m_pRigidBody = pBody; }

public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();
private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
public:
	// Create an instance
	static PhysicsObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
};

#endif //_PHYSICSOBJECT_H_