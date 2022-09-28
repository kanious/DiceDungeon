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


	// Renderer는 매니저로 뺐음
	// Dynamic Mesh는 STATIC으로 넣어서 자동 업데이트가 안되는데
	// 수동으로 업데이트해줘야 하는 경우가 있네?
	// 아 모델링 변경했을 때 변경한 애만 움직여줘야 하니까 그랬구나
	// aniExecutor가 body type 정보 들고 움직여줌
	// 애니메이션이랑 모델링은 상호작용 하도록 작성하고
	// 어쨌든 둘 다 map에 넣어놓고 자동 업데이트하도록
	
	// 셰이더는 Update가 아예 없고 렌더 기능만 있으니까 컴포넌트 말고 아예 따로 빼자
	// CShader로 작성, 하나만 쓰면 변수 하나로, 여러개 쓰면 맵으로

	// AniExecutor_Player 클래스는 어떤 상황에서 어떤 애니메이션을 플레이할 지
	// 애니메이션을 바꿔주는 클래스
	// 완전 하드코딩해놓은듯
	// 얘를 어떻게 잘 감싸서 구조를 짤지 고민해보기

	//_float				m_viewDepthZ;
	//_float				m_renderPriorityZ;
	// 얘네는 렌더 옵션인데 나중에 다시 공부한 다음 적용하자
	// 
	// viewDepthZ는 반투명 옵션 적용할 때 z버퍼값인듯 하고
	// Compute_ViewZ()에서 오브젝트 위치랑 카메라 위치 가지고 거리 계산하는 듯
	// 스틱스할 때 반투명을 썼었나 모르겠네...
	//
	// renderPriority는 렌더링 순서인 듯, 레이어에서 소팅할 때 사용
	// 왜 필요한지는 기억이 안 남. 렌더링을 먼저 해야되는 경우가 있었던가

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