#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include "Base.h"
#include "glm\vec3.hpp"

class SceneFPS;
class Firework;

class ParticleSystem : public Engine::CBase
{
	SINGLETON(ParticleSystem)

private:
	SceneFPS*					m_pScene;
	std::vector<Firework*>		m_vecObjs;

	_uint						m_iEffectCount;

private:
	explicit ParticleSystem();
	~ParticleSystem();

public:
	void SendEffect(glm::vec3 vPos);
	void ReturnEffect();

public:
	void Destroy();
	void Update(const _float& dt);
	RESULT Ready(SceneFPS* pScene);
};

#endif //_PARTICLESYSTEM_H_