#ifndef _MAPEDITORUI_H_
#define _MAPEDITORUI_H_

#include "Base.h"
#include "glm/vec3.hpp"

namespace Engine
{
	class CLayer;
	class CLight;
	class CGameObject;
	class CInputDevice;
}
class UIManager;
class Scene3D;

// Map editor UI class
class MapEditorUI : public Engine::CBase
{
private:
	class MeshInfo
	{
	public:
		std::string		meshId;
		std::string		meshType;
		char*			inputSize;
	};

private:
	Engine::CInputDevice*			m_pInputDevice;
	UIManager*						m_pUIManager;
	Scene3D*						m_pScene;

	std::vector<MeshInfo>			m_vecMeshInfos;

	// All Layer Enable flag
	_bool							m_bAllLayerShow;
	_bool							m_bAllLayerShowPrev;

	// Layer Type Combo box
	const char*						m_curLayerNameCombo;
	std::vector<const char*>		m_vecLayerNameCombo;

	// Mesh Type Combo box
	const char*						m_curMeshTypeCombo;
	std::vector<const char*>		m_vecMeshTypeCombo;

	// Target
	Engine::CGameObject*			m_pTarget;
	// For Target Obj Data
	char							m_chPos[3][128];
	char							m_chRot[3][128];
	char							m_chScale[3][128];
	glm::vec3 m_vPosPrev;
	glm::vec3 m_vRotPrev;
	glm::vec3 m_vScalePrev;
	// For Target Layer Combo box
	const char*						m_targetLayerNameCombo;

	// Scroll flag
	_bool							m_bScrollSet;

	// Snap flag
	_bool							m_bSnap;
	// Object move flag
	_bool							m_bObjMove;

	// All Object status flags
	_bool							m_bLock;
	_bool							m_bLockPrev;
	_bool							m_bShow;
	_bool							m_bShowPrev;
	_bool							m_bBBox;
	_bool							m_bBBoxPrev;
	_bool							m_bWire;
	_bool							m_bWirePrev;
	_bool							m_bAlpha;
	_bool							m_bAlphaPrev;

	// Copy object information
	_bool							m_bIsInformation;
	char							m_chCopiedPos[3][128];
	char							m_chCopiedRot[3][128];
	char							m_chCopiedScale[3][128];
	glm::vec3						m_vCopiedPos;
	glm::vec3						m_vCopiedRot;
	glm::vec3						m_vCopiedScale;


private:
	explicit MapEditorUI();
	~MapEditorUI();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Basic Update Function
	void Update(const _float& dt);
	// Basic Render Function
	void RenderUI();

public:
	// Select Target
	void SetTarget(Engine::CGameObject* pObj);
	// Remove Target
	void ResetTarget();

private:
	// Draw Menu Bar
	void SetMenuBar();
	// Draw Object List UI
	void Render_ObjectList(_float childX, _float childY);
	// Draw Mesh List UI
	void Render_MeshList(_float childX, _float childY);
	// Draw Inspector
	void Render_Inspector();
	// Draw Copy window (only draw when user copy the target information)
	void Render_CopyInformation();
	// Move target
	void MoveTarget(const _float& dt);
	// Check User input
	void KeyCheck(const _float& dt);

private:
	// Convert float to char*
	void ConvertFloatToCharArray(char* src, _float value);

private:
	// Initialize
	RESULT Ready(Scene3D* pScene);
public:
	// Create an instance
	static MapEditorUI* Create(Scene3D* pScene);
};

#endif //_MAPEDITORUI_H_