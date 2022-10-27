#ifndef _SCENE01_H_
#define _SCENE01_H_

#include "Define.h"
#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class TargetCamera;

class Scene01 : public Engine::CScene
{
private:
	Engine::CInputDevice*	m_pInputDevice;
	TargetCamera*			m_pTargetCamera;

private:
	explicit Scene01();
	virtual ~Scene01();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	void RestartBattle();
	void AddPlayerToRandomCoord(Engine::CLayer*);
	void AddEnemyToRandomCoord(Engine::CLayer*);

private:
	RESULT Ready(eSCENETAG);
	RESULT ReadyLayer(eSCENETAG);
	RESULT ReadyComponent();
public:
	static Scene01* Create(eSCENETAG);
};

#endif //_SCENE01_H_