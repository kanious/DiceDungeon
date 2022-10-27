#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include "Component.h"
#include "EngineStruct.h"

NAMESPACE_BEGIN(Engine)

class CVIBuffer;
class CTransform;

class ENGINE_API CAABB : public CComponent
{
public:
	CVIBuffer*			m_pVIBuffer;
	VTX*				m_pVertices;
	glm::vec3			m_vCenter;
	glm::vec3			m_vHalfExtents;
	glm::vec3			m_vMin;
	glm::vec3			m_vMax;
	glm::vec3			m_vMinWorld;
	glm::vec3			m_vMaxWorld;

private:
	explicit CAABB();
	explicit CAABB(const CAABB& rhs);
	virtual ~CAABB();
public:
	virtual void Render();
private:
	virtual void Destroy();

public:
	VTX* GetVertices()					{ return m_pVertices; }

private:
	RESULT Ready(glm::vec3 center, glm::vec3 half, glm::vec3 min, glm::vec3 max);
public:
	virtual CComponent* Clone();
	static CAABB* Create(glm::vec3 center, glm::vec3 half, glm::vec3 min, glm::vec3 max);
};

NAMESPACE_END

#endif //_COLLISIONMASTER_H_