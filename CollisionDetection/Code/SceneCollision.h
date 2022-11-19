#ifndef _SCENECOLLISION_H_
#define _SCENECOLLISION_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class BGObject;

class SceneCollision : public Engine::CScene
{
private:
	Engine::CInputDevice*	m_pInputDevice;
	DefaultCamera*			m_pDefaultCamera;
	BGObject*				m_pTerrain;

private:
	explicit SceneCollision();
	virtual ~SceneCollision();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT Ready();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneCollision* Create();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENECOLLISION_H_