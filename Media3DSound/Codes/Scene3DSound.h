#ifndef _SCENE3DSOUND_H_
#define _SCENE3DSOUND_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class BGObject;

class Scene3DSound : public Engine::CScene
{
private:
	Engine::CInputDevice*	m_pInputDevice;
	DefaultCamera*			m_pDefaultCamera;
	BGObject*				m_pTerrain;

	_float					m_fLightCoolTime;
	_float					m_fElapseTime;
	_bool					m_bRed;

	_bool					m_bDebugSphere;

private:
	explicit Scene3DSound();
	virtual ~Scene3DSound();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static Scene3DSound* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENE3DSOUND_H_