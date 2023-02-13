#ifndef _SCENE_H_
#define _SCENE_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CLayer;
class CGameObject;

//A scene class that holds game objects.
class ENGINE_API CScene : public CBase
{
protected:
	_uint							m_tag;
	std::vector<CLayer*>			m_vecLayer;

protected:
	std::string						m_DataPath;
	std::string						m_ObjListFileName;
	std::string						m_LightListFileName;

protected:
	explicit CScene();
	virtual ~CScene();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
protected:
	virtual void Destroy();

public:
	_uint GetSceneTag()								{ return m_tag; }
	std::string GetDataPath()						{ return m_DataPath; }
	std::string GetObjListFileName()				{ return m_ObjListFileName; }
	std::string GetLightListFileName()				{ return m_LightListFileName; }
	std::vector<CLayer*>& GetLayers()				{ return m_vecLayer; }

	void SetSceneTag(_uint sceneTag);
	void SetDataPath(std::string path)				{ m_DataPath = path; }
	void SetObjListFileName(std::string name)		{ m_ObjListFileName = name; }
	void SetLightListFileName(std::string name)		{ m_LightListFileName = name; }

	RESULT ToggleLayer(_uint layerTag, _bool turn);
	CLayer* GetLayer(_uint layerTag);
	RESULT AddLayer(_uint layerTag);
	RESULT AddGameObjectToLayer(_uint layerTag, CGameObject* pInstance);
};

NAMESPACE_END

#endif //_SCENE_H_