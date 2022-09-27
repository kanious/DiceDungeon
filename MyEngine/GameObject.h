#pragma once

#include "Defines.h"
#include "Base.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CGameObject : CBase
{
protected:
	_float			m_viewDepthZ;
	_float			m_renderPriority;

	// Object Key�� enum �ϳ� �ļ� ������Ʈ Ÿ�� �����ؾ� �� ��
	// ������Ʈ�� �⺻���� ��(Transform) ���⿡ �ް�
	// �޽� �� ��ü ���� �޶�� �ϴ� �� Ŭ�󿡼� �߰��ϱ�


protected:
	explicit CGameObject();
	virtual ~CGameObject();

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