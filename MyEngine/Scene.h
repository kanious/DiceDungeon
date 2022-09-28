#ifndef _SCENE_H_
#define _SCENE_H_

#include "Defines.h"
#include "Base.h"
#include "ObjectManager.h"

NAMESPACE_BEGIN(Engine)

class CObjectManager;

class ENGINE_API CScene : public CBase
{
protected:
	eSCENE				m_tag;
	CObjectManager*		m_pObjectManager;

public:
	explicit CScene();
	virtual ~CScene();

protected:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();

public:
	void SetSceneTag(eSCENE);

};

NAMESPACE_END

#endif //_SCENE_H_