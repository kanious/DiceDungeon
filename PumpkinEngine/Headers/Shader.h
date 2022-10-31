#ifndef _SHADER_H_
#define _SHADER_H_

#include "Component.h"
#include "glm/mat4x4.hpp"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CShader : public CComponent
{
protected:
	_uint				m_ShaderProgram;
	_uint				m_matWorldLocation;
	_uint				m_matViewLocation;
	_uint				m_matProjLocation;
	_uint				m_diffTexLocation;
	_uint				m_lightEnableLocation;
	_uint				m_selectedLocation;

private:
	explicit CShader();
	explicit CShader(const CShader& rhs);
	virtual ~CShader();
public:
	virtual void Render();
private:
	virtual void Destroy();

private:
	RESULT CreateShaderProgram();
	RESULT AddShader(const char* vertexPath, const char* fragPath);
	std::string ReadShader(const char* fileName);
	_uint CreateShader(_uint shaderType, std::string source);
	void SetLocation();
public:
	_uint GetShaderProgram()		{ return m_ShaderProgram; }
	void SetMatrixInfo(const glm::mat4x4 world, const glm::mat4x4 view, const glm::mat4x4 proj);
	void SetTextureInfo();
	void SetLightEnableInfo(_bool lightEnable);
	void SetSelected(_bool selected);

private:
	RESULT Ready(std::string ID, const char* vertexPath, const char* fragPath);
public:
	virtual CComponent* Clone();
	static CShader* Create(std::string ID, const char* vertexPath, const char* fragPath);
};

NAMESPACE_END

#endif //_SHADER_H_