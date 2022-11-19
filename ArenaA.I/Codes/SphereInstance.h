#ifndef _SPHEREINSTANCE_H_
#define _SPHEREINSTANCE_H_

#include "GameObject.h"

namespace Engine
{
	class CMesh;
}

class SphereInstance : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;
	_bool						m_bMain;

	_float						m_fAliveTime;
	_float						m_fAliveTimeElapse;

	glm::vec3					m_vOriginPos;
	glm::vec3					m_vTargetPos;
	_float						m_fSpeed;

	_float						m_fChildCount;

private:
	explicit SphereInstance();
	virtual ~SphereInstance();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	void MakeChild(_uint num, glm::vec3 vDir);

private:
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _bool isMain, glm::vec3 targetPos);
public:
	static SphereInstance* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _bool isMain, glm::vec3 targetPos);
};


#endif //_SPHEREINSTANCE_H_