#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

#include "GameObject.h"
#include "EngineStruct.h"

namespace Engine
{
	class CMesh;
}

// Static Object class
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;

private:
	explicit BGObject();
	virtual ~BGObject();
	// Call instead of destructor to manage class internal data
	virtual void Destroy();
public:
	// Basic Update Function
	virtual void Update(const _float& dt);
	// Basic Render Function
	virtual void Render();

public:
	// Return mesh type
	virtual const std::string GetMeshType();

private:
	// Initialize
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
public:
	// Create an instance
	static BGObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
};

#endif //_BGOBJECT_H_