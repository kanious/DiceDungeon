#include "ConfigurationManager.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"

SINGLETON_FUNCTION(ConfigurationManager)

ConfigurationManager::ConfigurationManager()
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_fFov = 0.f;
	m_fRatio = 0.f;
	m_fNear = 0.f;
	m_fFar = 0.f;
	m_matView = mat4x4(1.f);
	m_matProj = mat4x4(1.f);
	m_fGravity = 9.8f;
}

ConfigurationManager::~ConfigurationManager()
{
}

void ConfigurationManager::Destroy()
{
	DestroyInstance();
}

void ConfigurationManager::InitialSetting()
{
	m_fFov = 0.6f;
	SetRatio();
	m_fNear = 0.1f;
	m_fFar = 1000.0f;
}

void ConfigurationManager::SetFieldOfView(_float fov)
{
	m_fFov = fov;
}

void ConfigurationManager::SetRatio()
{
	glfwGetFramebufferSize(Engine::COpenGLDevice::GetInstance()->GetWindow(), &m_iWidth, &m_iHeight);
	m_fRatio = m_iWidth / (_float)m_iHeight;
}

void ConfigurationManager::SetNear(_float fNear)
{
	m_fNear = fNear;
}

void ConfigurationManager::SetFar(_float fFar)
{
	m_fFar = fFar;
}

void ConfigurationManager::SetViewMatrix(mat4x4 matView)
{
	m_matView = matView;
}

void ConfigurationManager::SetProjMatrix(mat4x4 matProj)
{
	m_matProj = matProj;
}
