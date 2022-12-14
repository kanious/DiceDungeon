#ifndef _TARGET_H_
#define _TARGET_H_

#include "GameObject.h"
#include "EngineStruct.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CMesh;
}

class Target : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	std::string						m_soundTag;
	_bool							m_bHovered;
	_float							m_fHoverElapsed;
	_float							m_fHoverTime;

	_float							m_fMoveElapsed;
	_float							m_fMoveTime;
	glm::vec3						m_vTargetDir;

	_float							m_fSpeed;
	_float							m_fAccel;
	_float							m_fMaxSpeed;

private:
	explicit Target();
	virtual ~Target();

public:
	virtual void Update(const _float& dt);
	virtual void Render();

public:
	void Boom();
	void Hovered(_bool value)		{ m_bHovered = value; }
	_bool GetHover()				{ return m_bHovered; }
	_float GetHoverElaspedTime()	{ return m_fHoverElapsed; }
private:
	void GetNewLocation();
	_bool CheckBoundary(glm::vec3 vPos);

private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string soundTag);
public:
	static Target* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, std::string soundTag);
};

#endif //_TARGET_H_