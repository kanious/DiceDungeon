#ifndef _GAMEMASTER_H_
#define _GAMEMASTER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CScene;
class CRenderer;

// A super manager class that updates scenes and renders the renderer.
class ENGINE_API CGameMaster : public CBase
{
	SINGLETON(CGameMaster)

private:
	CScene*				m_pCurrentScene;
	CRenderer*			m_pRenderer;
	_bool				m_bGameClose;
	std::string			m_strDataPath;

private:
	explicit CGameMaster();
	virtual ~CGameMaster();
public:
	// Basic Update Function, update current scene
	void Update(const _float& dt);
	// Basic Render Function, render renderer (and scene if needed)
	void Render();
private:
	// Call instead of destructor to manage class internal data
	void Destroy();

public:
	CScene* GetCurrentScene()				{ return m_pCurrentScene; }
	// Return game close status
	_bool GetGameClose()					{ return m_bGameClose; }
	// Get asset folder path
	std::string GetDataPath()				{ return m_strDataPath; }
	// Set current scene information
	void SetCurrentScene(CScene* pScene);
	// Set game close status
	void SetGameClose(_bool value)			{ m_bGameClose = value; }
	// Set asset folder path
	void SetAssetPath(std::string path)		{ m_strDataPath = path; }

public:
	// Initialize
	void Ready();
	// Set RenderType
	void SetRenderType(_bool deferred);
};

NAMESPACE_END

#endif //_GAMEMASTER_H_