#pragma once

#include "Defines.h"
#include "Base.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;

class ENGINE_API CLayer : CBase
{
private:
	eLAYER					m_tag = LAYER_END;
	vector<CGameObject*>	m_vecGameObjects;

protected:
	explicit CLayer();
	virtual ~CLayer();

private:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();

public:
	void SetTag(eLAYER);
	eLAYER GetTag();
	void AddGameObject(CGameObject*);

public:
	static CLayer* Create(eLAYER);
};

NAMESPACE_END