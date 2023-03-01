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


private:
	explicit MapEditorUI();
	~MapEditorUI();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();

public:
	void SetTarget(Engine::CGameObject* pObj);
	void ResetTarget();

private:
	void SetMenuBar();
	void Render_ObjectList(_float childX, _float childY);
	void Render_MeshList(_float childX, _float childY);
	void Render_Inspector();
	void MoveTarget(const _float& dt);
	void KeyCheck(const _float& dt);

private:
	void ConvertFloatToCharArray(char* src, _float value);

private:
	RESULT Ready(Scene3D* pScene);
public:
	static MapEditorUI* Create(Scene3D* pScene);
};

#endif //_MAPEDITORUI_H_