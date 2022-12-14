#include "ParticleSystem.h"
#include "OpenGLDefines.h"
#include "SceneFPS.h"
#include "Define.h"
#include "Function.h"
#include "Firework.h"


SINGLETON_FUNCTION(ParticleSystem)
USING(Engine)
USING(glm)
USING(std)

ParticleSystem::ParticleSystem()
	: m_pScene(nullptr), m_iEffectCount(0)
{
	m_vecObjs.clear();
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::SendEffect(vec3 vPos)
{
	if (m_iEffectCount < 20)
		return;

	int iCount = 0;
	for (int i = 0; i < m_vecObjs.size(); ++i)
	{
		if (!m_vecObjs[i]->GetEnable())
		{
			_float xValue = GetRandNum(-100, 100) / 100.f;
			_float yValue = GetRandNum(-100, 100) / 100.f;
			_float zValue = GetRandNum(-100, 100) / 100.f;

			_float speed = GetRandNum(100, 150) / 1000.f;
			_float accel = GetRandNum(500, 800) / 10000.f;
			_float age = GetRandNum(400, 600) / 1000.f;

			m_vecObjs[i]->SendEffect(vPos, vec3(xValue, yValue, zValue), speed, accel, age);

			++iCount;
			--m_iEffectCount;
			if (iCount > 19)
				return;
		}
	}
}

void ParticleSystem::ReturnEffect()
{
	++m_iEffectCount;
}

void ParticleSystem::Destroy()
{
	for (int i = 0; i < m_vecObjs.size(); ++i)
		SafeDestroy(m_vecObjs[i]);
	m_vecObjs.clear();
}

void ParticleSystem::Update(const _float& dt)
{
	for (int i = 0; i < m_vecObjs.size(); ++i)
		m_vecObjs[i]->Update(dt);
}

RESULT ParticleSystem::Ready(SceneFPS* pScene)
{
	m_pScene = pScene;
	m_iEffectCount = PARTICLE_COUNT;

	for (int i = 0; i < PARTICLE_COUNT; ++i)
	{
		Firework* obj = Firework::Create("Sphere", vec3(0.f), vec3(0.01f));
		if (nullptr != obj)
			m_vecObjs.push_back(obj);
	}

	return PK_NOERROR;
}
