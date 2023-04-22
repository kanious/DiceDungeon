#ifndef _TARGETTEXTURE_H_
#define _TARGETTEXTURE_H_

#include "Component.h"
#include "glm\mat4x4.hpp"

NAMESPACE_BEGIN(Engine)

class CShader;
// Component for texture data
class ENGINE_API CTargetTexture : public CComponent
{
private:
	_uint				m_iTextureID;

	_uint				m_iVAO_ID;
	_uint				m_iVB_ID;

	_uint				m_iIdx;

	CShader*			m_pShader;

	glm::mat4x4			m_matWorld;

private:
	explicit CTargetTexture();
	explicit CTargetTexture(const CTargetTexture& rhs);
	virtual ~CTargetTexture();

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	_uint GetTextureID() { return m_iTextureID; }
	void Render();

private:
	// Load texture information from file
	RESULT Ready(std::string ID, _uint texID, glm::vec3 vPos);
	void SetVIBuffer();
	void SetShader();
public:
	// Clone component
	virtual CComponent* Clone();
	// Create an instance
	static CTargetTexture* Create(std::string ID, _uint texID, glm::vec3 vPos);
};

NAMESPACE_END

#endif //_TARGETTEXTURE_H_