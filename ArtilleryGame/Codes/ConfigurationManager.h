#ifndef _CONFIGURATIONMANAGER_H_
#define _CONFIGURATIONMANAGER_H_

#include "Define.h"
#include "OpenGLDefines.h"

class ConfigurationManager
{
	SINGLETON(ConfigurationManager)

private:
	// Graphic
	_int					m_iWidth;
	_int					m_iHeight;
	_float					m_fFov;
	_float					m_fRatio;
	_float					m_fNear;
	_float					m_fFar;
	mat4x4					m_matView;
	mat4x4					m_matProj;

	// Physics
	_float					m_fGravity;

private:
	explicit ConfigurationManager();
	~ConfigurationManager();
public:
	void Destroy();

public:
	void InitialSetting();
	const _int GetWidth()				{ return m_iWidth;  }
	const _int GetHeight()				{ return m_iHeight;  }
	const _float GetFieldOfView()		{ return m_fFov; }
	const _float GetRatio()				{ SetRatio(); return m_fRatio; }
	const _float GetNear()				{ return m_fNear; }
	const _float GetFar()				{ return m_fFar; }
	const mat4x4 GetViewMatrix()		{ return m_matView; }
	const mat4x4 GetProjMatrix()		{ return m_matProj; }
	const _float GetGravity()			{ return m_fGravity; }
	void SetFieldOfView(_float);
	void SetRatio();
	void SetNear(_float);
	void SetFar(_float);
	void SetViewMatrix(mat4x4);
	void SetProjMatrix(mat4x4);
};

#endif //_CONFIGURATIONMANAGER_H_