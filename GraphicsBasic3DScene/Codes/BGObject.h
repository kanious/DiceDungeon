#ifndef _TREE_H_
#define _TREE_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CAABB;
}
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;

private:
	explicit BGObject();
	virtual ~BGObject();

public:
	std::string GetMeshID();
	void SetSelected(_bool select);

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
public:
	static BGObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);
	virtual CGameObject* Clone();
};

#endif //_TREE_H_