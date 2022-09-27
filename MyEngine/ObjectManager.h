#pragma once

#include "Defines.h"
#include "Layer.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CLayer;
class CGameObject;

class ENGINE_API CObjectManager
{
	SINGLETON(CObjectManager)

private:
	typedef unordered_map<eLAYER, CLayer*>		LAYER_MAP;
	typedef unordered_map<eSCENE, LAYER_MAP*>	SCENE_MAP;
	SCENE_MAP									m_mapLayer;


private:
	explicit CObjectManager();
	~CObjectManager();

private:
	void Awake();
public:
	void Update(const _float&, eSCENE);
	void Destroy();

public:
	void AddGameObject(eSCENE, eLAYER, CGameObject*);
};

NAMESPACE_END