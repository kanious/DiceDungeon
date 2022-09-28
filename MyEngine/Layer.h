#ifndef _LAYER_H_
#define _LAYER_H_

#include "Defines.h"
#include "Base.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CGameObject;

class ENGINE_API CLayer : public CBase
{
protected:
	eLAYER					m_tag = LAYER_END;
	vector<CGameObject*>	m_vecGameObjects;

protected:
	explicit CLayer();
	virtual ~CLayer();

protected:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetTag(eLAYER);
	eLAYER GetTag();
	void AddGameObject(CGameObject*);

public:
	static CLayer* Create(eLAYER);
};

NAMESPACE_END

#endif //_LAYER_H_