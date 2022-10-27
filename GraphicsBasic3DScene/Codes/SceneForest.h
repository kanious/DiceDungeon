#ifndef _SCENEFOREST_H_
#define _SCENEFOREST_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class Tree;
class SceneForest : public Engine::CScene
{
private:
	Engine::CInputDevice*			m_pInputDevice;
	DefaultCamera*					m_pDefaultCamera;
	Tree*							m_pTargetObject;
	Engine::CLayer*					m_pBackgroundLayer;


	_bool m_bMapEditorUIOpened = true;
	_bool m_bSoundUIOpened = false;

private:
	explicit SceneForest();
	virtual ~SceneForest();

private:
	void KeyCheck(const _float& dt);

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	
private:
	RESULT Ready();
	RESULT ReadyComponent();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneForest* Create();
};

#endif //_SCENEFOREST_H_