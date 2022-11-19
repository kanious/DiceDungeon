#ifndef _SPHEREROCKET_H_
#define _SPHEREROCKET_H_

#include "GameObject.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CMesh;
	class CQuadTree;
}

class SphereRocket : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;
	_uint						m_iMasterNum;
	Engine::CQuadTree*			m_pQuadTree;

	glm::vec3					m_vDir;;
	glm::vec3					m_vBombPos;


private:
	explicit SphereRocket();
	virtual ~SphereRocket();

public:
	void SetQuadTree(Engine::CQuadTree* qt) { m_pQuadTree = qt; }
	_bool RobotCollisionCheck();
	_bool CollisionCheck();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _uint masterNum, glm::vec3 vDir);
public:
	static SphereRocket* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _uint masterNum, glm::vec3 vDir);
};


#endif //_SPHEREROCKET_H_