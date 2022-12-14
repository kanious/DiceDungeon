#ifndef _SCENE3DSOUND_H_
#define _SCENE3DSOUND_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
	class CSkyBox;
}
class PhysicsSystem;
class ParticleSystem;
class UIManager;
class DefaultCamera;
class BGObject;

class SceneFPS : public Engine::CScene
{
private:
	Engine::CInputDevice*	m_pInputDevice;
	PhysicsSystem*			m_pPhysicsSystem;
	ParticleSystem*			m_pParticleSystem;
	UIManager*				m_pUIManager;

	DefaultCamera*			m_pDefaultCamera;
	Engine::CLayer*			m_pObjLayer;

	Engine::CSkyBox*		m_pSkyBox;

private:
	explicit SceneFPS();
	virtual ~SceneFPS();
	
private:
	void KeyCheck();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneFPS* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENE3DSOUND_H_