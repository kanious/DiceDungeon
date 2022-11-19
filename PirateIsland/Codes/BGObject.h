#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

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
	std::string						m_soundTag;
	glm::vec3						m_vDestPos;
	glm::vec3						m_vMiddlePos;
	glm::vec3						m_vDir;
	_float							m_fSpeed;
	_bool							m_bMoving;
	_bool							m_bArriving;
	_bool							m_bMiddlePoint;

	_bool							m_bLightTurnOnDelay;
	_bool							m_bLightTurnOffDelay;
	_float							m_fDelay;
	_float							m_fElapseTime;

private:
	explicit BGObject();
	virtual ~BGObject();

public:
	void SetMoving(glm::vec3 vStartPos, glm::vec3 vDestPos, _bool mode, _bool middle = false, glm::vec3 vMiddlePos = glm::vec3(0.f));
	void Moving(const _float& dt);
private:
	void RotationCheck(const _float& dt);

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