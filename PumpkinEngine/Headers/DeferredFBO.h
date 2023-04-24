#ifndef _DEFERREDFBO_H_
#define _DEFERREDFBO_H_

#include "Component.h"

NAMESPACE_BEGIN(Engine)

class CShader;
class CTargetTexture;
class CTexture;
// Component for frame buffer object data
class ENGINE_API CDeferredFBO : public CComponent
{
private:
	_uint				m_iFrameBufferID;
	_uint				m_iDepthID;
	_uint				m_iDiffuseTexture;
	_uint				m_iPositionTexture;
	_uint				m_iNormalTexture;

	_int				m_iWidth;
	_int				m_iHeight;

	_uint				m_iVAO_ID;
	_uint				m_iVB_ID;

	CShader*			m_pShader;

	CTargetTexture*		m_pDiffuseTarget;
	CTargetTexture*		m_pNormalTarget;
	CTargetTexture*		m_pPosTarget;

	CTexture*			m_pNoiseTex;
	_bool				m_bIsNoise;
	_float				m_fNoiseAmount;

private:
	explicit CDeferredFBO();
	explicit CDeferredFBO(const CDeferredFBO& rhs);
	virtual ~CDeferredFBO();

private:
	// Call instead of destructor to manage class internal data
	virtual void Destroy();

public:
	_int GetWidth()						{ return m_iWidth; }
	_int GetHeight()					{ return m_iHeight; }
	_bool GetNoise()					{ return m_bIsNoise; }
	void SetNoise(_bool value)			{ m_bIsNoise = value; }
	_float GetNoiseAmount()				{ return m_fNoiseAmount; }
	void SetNoiseAmount(_float value)	{ m_fNoiseAmount = value; }

private:
	// Load texture information from file
	RESULT Ready(std::string ID, _int width, _int height);
	RESULT SetVIBuffer();
	RESULT SetShader();
	RESULT SetFrameBuffer();
public:
	void RenderStart();
	void Render();
	void RenderEnd();
	void Render_Debug();

public:
	// Clone component
	virtual CComponent* Clone();
	// Create an instance
	static CDeferredFBO* Create(std::string ID, _int width, _int height);
};

NAMESPACE_END

#endif //_DEFERREDFBO_H_