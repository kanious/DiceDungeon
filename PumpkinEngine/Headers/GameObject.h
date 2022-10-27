#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Base.h"
#include "glm\vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CComponent;
class CRenderer;
class CLayer;
class CTransform;
class CAABB;

class ENGINE_API CGameObject : public CBase
{
protected:
	_wchar_t*							m_UUID;
	_uint								m_objTag;
	_uint								m_layerTag;
	_uint								m_sceneTag;
	_bool								m_bEnable;
	_bool								m_bDead;

	typedef std::unordered_map<std::string, CComponent*> COMPONENT_MAP;
	COMPONENT_MAP						m_mapComponent;
	CRenderer*							m_pRenderer;
	CLayer*								m_pLayer;
	CTransform*							m_pTransform;
	CAABB*								m_pBoundingBox_AABB;

protected:
	explicit CGameObject();
	virtual ~CGameObject();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
	void Enable();
	void Disable();
protected:
	virtual void Destroy();

public:
	void SetupGameObject(_uint sceneTag, _uint layerTag, _uint objTag);
	const _wchar_t* GetUUID()				{ return m_UUID; }
	const _uint GetObjectTag()				{ return m_objTag; }
	const _uint GetLayerTag()				{ return m_layerTag; }
	const _uint GetSceneTag()				{ return m_sceneTag; }
	const _bool GetEnable()					{ return m_bEnable; }
	const _bool GetDead()					{ return m_bDead; }
	CAABB* GetBoundingBox_AABB()			{ return m_pBoundingBox_AABB; }
	glm::vec3 GetPosition();
	_float GetRotationY();
	glm::vec3 GetScale();
	void SetObjectTag(_uint objTag);
	void SetLayerTag(_uint layerTag);
	void SetSceneTag(_uint sceneTag);
	void SetParentLayer(CLayer* pLayer);
	void SetDead(_bool dead);
	void SetPosition(glm::vec3 vPos);
	void SetRotationY(_float fAngle);
	void SetScale(glm::vec3 vScale);
public:
	void AttachComponent(std::string componentTag, CComponent* pInstance);
	RESULT RemoveComponent(std::string componentTag);
	CComponent* GetComponent(std::string componentTag);

public:
	virtual CGameObject* Clone() = 0;
private:
	_wchar_t* UUIDGenerate();
};

NAMESPACE_END

#endif //_GAMEOBJECT_H_