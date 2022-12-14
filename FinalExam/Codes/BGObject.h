#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

#include "GameObject.h"
#include "EngineStruct.h"

namespace Engine
{
	class CMesh;
}
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	std::string						m_soundTag;

private:
	explicit BGObject();
	virtual ~BGObject();

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