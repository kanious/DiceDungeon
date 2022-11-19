#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

#include "GameObject.h"
#include "EngineStruct.h"

namespace Engine
{
	class CMesh;
	class COctree;
}
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;

private:
	explicit BGObject();
	virtual ~BGObject();

public:
	Engine::TRIANGLE* GetTrianglesFromMesh();
	_uint GetTriangleNumber();
	Engine::COctree* GetOctreeFromMesh();
	void SetupOctree(_uint depth);

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string soundTag);
public:
	static BGObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string soundTag);
};

#endif //_BGOBJECT_H_