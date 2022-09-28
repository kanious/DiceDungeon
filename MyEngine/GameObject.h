#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Defines.h"
#include "Base.h"
#include "Component.h"
#include "Utils.h"

NAMESPACE_BEGIN(Engine)

class CComponent;

class ENGINE_API CGameObject : public CBase
{
protected:
	_wchar_t*			m_UUID;
	eOBJTYPE			m_eType;
	_bool				m_bEnable;
	CComponent*			m_CompArr[COMP_END];


	// Renderer�� �Ŵ����� ����
	// Dynamic Mesh�� STATIC���� �־ �ڵ� ������Ʈ�� �ȵǴµ�
	// �������� ������Ʈ����� �ϴ� ��찡 �ֳ�?
	// �� �𵨸� �������� �� ������ �ָ� ��������� �ϴϱ� �׷�����
	// aniExecutor�� body type ���� ��� ��������
	// �ִϸ��̼��̶� �𵨸��� ��ȣ�ۿ� �ϵ��� �ۼ��ϰ�
	// ��·�� �� �� map�� �־���� �ڵ� ������Ʈ�ϵ���
	
	// ���̴��� Update�� �ƿ� ���� ���� ��ɸ� �����ϱ� ������Ʈ ���� �ƿ� ���� ����
	// CShader�� �ۼ�, �ϳ��� ���� ���� �ϳ���, ������ ���� ������

	// AniExecutor_Player Ŭ������ � ��Ȳ���� � �ִϸ��̼��� �÷����� ��
	// �ִϸ��̼��� �ٲ��ִ� Ŭ����
	// ���� �ϵ��ڵ��س�����
	// �긦 ��� �� ���μ� ������ ©�� ����غ���

	//_float				m_viewDepthZ;
	//_float				m_renderPriorityZ;
	// ��״� ���� �ɼ��ε� ���߿� �ٽ� ������ ���� ��������
	// 
	// viewDepthZ�� ������ �ɼ� ������ �� z���۰��ε� �ϰ�
	// Compute_ViewZ()���� ������Ʈ ��ġ�� ī�޶� ��ġ ������ �Ÿ� ����ϴ� ��
	// ��ƽ���� �� �������� ����� �𸣰ڳ�...
	//
	// renderPriority�� ������ ������ ��, ���̾�� ������ �� ���
	// �� �ʿ������� ����� �� ��. �������� ���� �ؾߵǴ� ��찡 �־�����

protected:
	explicit CGameObject();
	virtual ~CGameObject();

protected:
	virtual void Awake();
public:
	virtual void Update(const _float&);
	virtual void Render();
	virtual void Destroy();
	void Enable();
	void Disable();

public:
	const _wchar_t* GetUUID()		{ return m_UUID; }
	const _bool GetEnable()			{ return m_bEnable; }
	const eOBJTYPE GetObjectType()	{ return m_eType; }
	void SetObjectType(eOBJTYPE);
	void AttachComponent(eCOMPONENT, CComponent*);
	void DetachComponent(eCOMPONENT);
	CComponent* GetComponent(eCOMPONENT);
};

NAMESPACE_END

#endif //_GAMEOBJECT_H_