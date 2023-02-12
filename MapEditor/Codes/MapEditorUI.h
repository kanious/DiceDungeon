#ifndef _MAPEDITORUI_H_
#define _MAPEDITORUI_H_

#include "Base.h"
#include "Enums.h"

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

	// Mesh Type Combo box
	const char*						m_curMeshTypeCombo;
	std::vector<const char*>		m_vecMeshTypeCombo;

	// Target
	Engine::CGameObject*			m_pTarget;
	// For Target Obj Data
	char							m_chPos[3][128];
	char							m_chRot[3][128];
	char							m_chScale[3][128];

private:
	explicit MapEditorUI();
	~MapEditorUI();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();

public:
	void SetTarget(Engine::CGameObject* pObj) { m_pTarget = pObj; }

private:
	void SetMenuBar();
	void Render_ObjectList(_float childX, _float childY);
	void Render_MeshList(_float childX, _float childY);
	void Render_Inspector();
	void KeyCheck(const _float& dt);

private:
	std::string GetLayerTagByIndex(eLAYERTAG tag);
	void ConvertFloatToCharArray(char* src, _float value);

private:
	RESULT Ready(Scene3D* pScene);
public:
	static MapEditorUI* Create(Scene3D* pScene);
};

#endif //_MAPEDITORUI_H_