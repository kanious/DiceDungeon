#ifndef _SCENE01_H_
#define _SCENE01_H_

#include "Define.h"
#include "Scene.h"

class TargetCamera;

class Scene01 : public Engine::CScene
{
private:
	TargetCamera* m_pTargetCamera;


private:
	explicit Scene01();
	virtual ~Scene01();

public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

private:
	RESULT Ready(eSCENETAG);
public:
	static Scene01* Create(eSCENETAG);
};

#endif //_SCENE01_H_