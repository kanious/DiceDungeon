#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "Defines.h"
#include "Layer.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CLayer;
class CGameObject;

// 데이터 담는 클래스는 따로 작업하고
// 여기서는 현재 씬의 오브젝트만 관리하기
// 해당 씬에서 사라지면 컨테이너에서도 빼버리기
// 잠시 사라지는 건 각 오브젝트의 Enable/Disable에서 관리

class ENGINE_API CObjectManager
{
	SINGLETON(CObjectManager)

private:
	list<CGameObject*>			m_listGameObjects[LAYER_END];

private:
	explicit CObjectManager();
	~CObjectManager();

private:
	void Awake();
public:
	void Update(const _float&);
	void Destroy();
	void DestroyManager();

public:
	void AddGameObject(eLAYER, CGameObject*);
	void RemoveGameObject(eLAYER, _wchar_t*);
	void RemoveAllObject();
};

NAMESPACE_END

#endif //_OBJECTMANAGER_H_