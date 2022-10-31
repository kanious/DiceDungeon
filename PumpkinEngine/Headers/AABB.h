#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_

#include "Component.h"
#include "EngineStruct.h"

NAMESPACE_BEGIN(Engine)

class CVIBuffer;
class COpenGLDevice;
class CShader;

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
	std::string			m_shaderID;

	COpenGLDevice*		m_pOpenGLDevice;
	CShader*			m_pShader;
	CTransform*			m_pParentTransform;

private:
	explicit CAABB();
	explicit CAABB(const CAABB& rhs);
	virtual ~CAABB();
public:
	virtual void Render();
private:
	virtual void Destroy();

public:
	VTX* GetVertices()							{ return m_pVertices; }
	void SetTransform(CTransform* transform)	{ m_pParentTransform = transform; }

private:
	RESULT Ready(glm::vec3 center, glm::vec3 half, glm::vec3 min, glm::vec3 max, std::string shaderID);
public:
	virtual CComponent* Clone();
	static CAABB* Create(glm::vec3 center, glm::vec3 half, glm::vec3 min, glm::vec3 max, std::string shaderID);
};

NAMESPACE_END

#endif //_COLLISIONMASTER_H_