#ifndef _FIREWORK_H_
#define _FIREWORK_H_

#include "GameObject.h"
#include "EngineStruct.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CMesh;
}

class Firework : public Engine::CGameObject
{
private:
	Engine::CMesh*				m_pMesh;

	glm::vec3					m_vDir;
	glm::vec3					m_vVelocity;
	glm::vec3					m_vForce;
	_float						m_fSpeed;
	_float						m_fAccel;
	_float						m_fAgeTime;
	_float						m_fElaspedTime;

private:
	explicit Firework();
	virtual ~Firework();

public:
	virtual void Update(const _float& dt);
	virtual void Render();

public:
	void SendEffect(glm::vec3 vPos, glm::vec3 vDir, _float speed, _float accel, _float age);

private:
	virtual void Destroy();
	RESULT Ready(std::string meshID, glm::vec3 vPos, glm::vec3 vScale);
public:
	static Firework* Create(std::string meshID, glm::vec3 vPos, glm::vec3 vScale);
};

#endif //_FIREWORK_H_