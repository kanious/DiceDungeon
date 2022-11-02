#ifndef _SCENE_H_
#define _SCENE_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CLayer;
class CGameObject;

class ENGINE_API CScene : public CBase
{
protected:
	_uint							m_tag;
	std::vector<CLayer*>			m_vecLayer;

protected:
	std::string						m_DataPath;
	std::string						m_SoundDataFileName;
	std::string						m_ShaderDataFileName;
	std::string						m_TextureDataFileName;
	std::string						m_MeshDataFileName;
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
	std::string GetSoundDataFileName()				{ return m_SoundDataFileName; }
	std::string GetShaderDataFileName()				{ return m_ShaderDataFileName; }
	std::string GetTextureDataFileName()			{ return m_TextureDataFileName; }
	std::string GetMeshDataFileName()				{ return m_MeshDataFileName; }
	std::string GetObjListFileName()				{ return m_ObjListFileName; }
	std::string GetLightListFileName()				{ return m_LightListFileName; }

	void SetSceneTag(_uint sceneTag);
	void SetDataPath(std::string path)				{ m_DataPath = path; }
	void SetSoundDataFileName(std::string name)		{ m_SoundDataFileName = name; }
	void SetShaderDataFileName(std::string name)	{ m_ShaderDataFileName = name; }
	void SetTextureDataFileName(std::string name)	{ m_TextureDataFileName = name; }
	void SetMeshDataFileName(std::string name)		{ m_MeshDataFileName = name; }
	void SetObjListFileName(std::string name)		{ m_ObjListFileName = name; }
	void SetLightListFileName(std::string name)		{ m_LightListFileName = name; }

	RESULT ToggleLayer(_uint layerTag, _bool turn);
	CLayer* GetLayer(_uint layerTag);
	RESULT AddGameObjectToLayer(_uint layerTag, CGameObject* pInstance);
	virtual void SaveBackgroundObjects();
	virtual void LoadBackgroundObjects();
};

NAMESPACE_END

#endif //_SCENE_H_