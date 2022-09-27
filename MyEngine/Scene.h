#pragma once

#include "Defines.h"
#include "Base.h"
#include "ObjectManager.h"

NAMESPACE_BEGIN(Engine)

class ObjectManager;

class ENGINE_API CScene : CBase
{
private:
	eSCENE				m_tag;
	ObjectManager*		m_pObjectManager;

protected:
	explicit CScene();
	virtual ~CScene();

private:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();

};

NAMESPACE_END