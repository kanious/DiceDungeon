#ifndef _SCENEARENA_H_
#define _SCENEARENA_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class BGObject;

class SceneArena : public Engine::CScene
{
private:
	Engine::CInputDevice*	m_pInputDevice;
	DefaultCamera*			m_pDefaultCamera;
	BGObject*				m_pTerrain;

private:
	explicit SceneArena();
	virtual ~SceneArena();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneArena* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENEARENA_H_