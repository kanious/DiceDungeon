#ifndef _BGOBJECT_H_
#define _BGOBJECT_H_

#include "Define.h"
#include "GameObject.h"

namespace Engine
{
	class CMesh;
	class CBoundingBox;
}
class BGObject : public Engine::CGameObject
{
private:
	Engine::CMesh*					m_pMesh;
	std::string						m_soundTag;

private:
	explicit BGObject();
	virtual ~BGObject();

public:
	std::string GetMeshID();
	std::string GetSoundTag()				{ return m_soundTag; }
	void SetSelected(_bool select);
	void SetSoundTag(std::string tag)		{ m_soundTag = tag; }
public:
	void PlaySound();

public:
	virtual void Update(const _float& dt);
	virtual void Render();
private:
	virtual void Destroy();
	RESULT Ready(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _int dir);
public:
	static BGObject* Create(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _int dir);
};

#endif //_BGOBJECT_H_