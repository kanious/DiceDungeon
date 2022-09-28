#ifndef _OBJECTMANAGER_H_
#define _OBJECTMANAGER_H_

#include "Defines.h"
#include "Layer.h"
#include "GameObject.h"

NAMESPACE_BEGIN(Engine)

class CLayer;
class CGameObject;

// ������ ��� Ŭ������ ���� �۾��ϰ�
// ���⼭�� ���� ���� ������Ʈ�� �����ϱ�
// �ش� ������ ������� �����̳ʿ����� ��������
// ��� ������� �� �� ������Ʈ�� Enable/Disable���� ����

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