#ifndef _SCENEFOREST_H_
#define _SCENEFOREST_H_

#include "Scene.h"

namespace Engine
{
	class CInputDevice;
	class CLayer;
}
class DefaultCamera;
class BGObject;
class UIManager;

class SceneForest : public Engine::CScene
{
private:
	Engine::CInputDevice*			m_pInputDevice;
	Engine::CLayer*					m_pBackgroundLayer;
	DefaultCamera*					m_pDefaultCamera;
	BGObject*						m_pTargetObject;
	UIManager*						m_pUIManager;

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

public:
	BGObject* GetTargetObj() { return m_pTargetObject; }

	
private:
	RESULT Ready();
	RESULT ReadyComponent();
	RESULT ReadyLayerAndGameObject();
public:
	static SceneForest* Create();
	virtual void SaveBackgroundObjects();
	virtual void LoadBackgroundObjects();
};

#endif //_SCENEFOREST_H_