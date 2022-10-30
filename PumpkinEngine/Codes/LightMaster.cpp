#include "pch.h"
#include "..\Headers\LightMaster.h"
#include "..\Headers\Light.h"
#include "..\Headers\OpenGLDefines.h"
#include <sstream>

USING(Engine)
USING(std)
SINGLETON_FUNCTION(CLightMaster)

CLightMaster::CLightMaster()
{
	m_mapLights.clear();
}

CLightMaster::~CLightMaster()
{
}

void CLightMaster::Destroy()
{
	LIGHT_MAP::iterator iter;
	for (iter = m_mapLights.begin(); iter != m_mapLights.end(); ++iter)
		SafeDestroy(iter->second);
	m_mapLights.clear();
}

RESULT CLightMaster::AddLight(string tag, CLight::cLightInfo* pInfo)
{
	LIGHT_MAP::iterator iter = m_mapLights.find(tag);
	if (iter != m_mapLights.end())
		return PK_LIGHT_EXIST;
	else
	{
		CLight* newLight = CLight::Create(pInfo);
		if (nullptr != newLight)
			m_mapLights.insert(LIGHT_MAP::value_type(tag, newLight));
	}

	return PK_NOERROR;
}

RESULT CLightMaster::RemoveLight(std::string tag)
{
	LIGHT_MAP::iterator iter = m_mapLights.find(tag);
	if (iter == m_mapLights.end())
		return PK_LIGHT_CANNOT_FIND;
	else
	{
		SafeDestroy(iter->second);
		m_mapLights.erase(iter);
	}

	return PK_NOERROR;
}

void CLightMaster::SetUniformLocation(_uint shaderID)
{
	_int index = 0;
	LIGHT_MAP::iterator iter;
	for (iter = m_mapLights.begin(); iter != m_mapLights.end(); ++iter)
	{
		stringstream ss;
		ss << "theLights[" << index << "].position";
		iter->second->SetLocation(0, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].direction";
		iter->second->SetLocation(1, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].diffuse";
		iter->second->SetLocation(2, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].specular";
		iter->second->SetLocation(3, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].ambient";
		iter->second->SetLocation(4, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].atten";
		iter->second->SetLocation(5, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].param1";
		iter->second->SetLocation(6, glGetUniformLocation(shaderID, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].param2";
		iter->second->SetLocation(7, glGetUniformLocation(shaderID, ss.str().c_str()));

		++index;
	}
}

void CLightMaster::SetLightInfo()
{
	LIGHT_MAP::iterator iter;
	for (iter = m_mapLights.begin(); iter != m_mapLights.end(); ++iter)
	{
		glUniform4f(iter->second->GetLocation(0)
			, iter->second->GetLightInfo()->position.x
			, iter->second->GetLightInfo()->position.y
			, iter->second->GetLightInfo()->position.z
			, iter->second->GetLightInfo()->position.w);

		glUniform4f(iter->second->GetLocation(1)
			, iter->second->GetLightInfo()->direction.x
			, iter->second->GetLightInfo()->direction.y
			, iter->second->GetLightInfo()->direction.z
			, iter->second->GetLightInfo()->direction.w);

		glUniform4f(iter->second->GetLocation(2)
			, iter->second->GetLightInfo()->diffuse.x
			, iter->second->GetLightInfo()->diffuse.y
			, iter->second->GetLightInfo()->diffuse.z
			, iter->second->GetLightInfo()->diffuse.w);

		glUniform4f(iter->second->GetLocation(3)
			, iter->second->GetLightInfo()->specular.x
			, iter->second->GetLightInfo()->specular.y
			, iter->second->GetLightInfo()->specular.z
			, iter->second->GetLightInfo()->specular.w);

		glUniform4f(iter->second->GetLocation(4)
			, iter->second->GetLightInfo()->ambient.x
			, iter->second->GetLightInfo()->ambient.y
			, iter->second->GetLightInfo()->ambient.z
			, iter->second->GetLightInfo()->ambient.w);

		glUniform4f(iter->second->GetLocation(5)
			, iter->second->GetLightInfo()->atten.x
			, iter->second->GetLightInfo()->atten.y
			, iter->second->GetLightInfo()->atten.z
			, iter->second->GetLightInfo()->atten.w);

		glUniform4f(iter->second->GetLocation(6)
			, iter->second->GetLightInfo()->param1.x
			, iter->second->GetLightInfo()->param1.y
			, iter->second->GetLightInfo()->param1.z
			, iter->second->GetLightInfo()->param1.w);

		glUniform4f(iter->second->GetLocation(7)
			, iter->second->GetLightInfo()->param2.x
			, iter->second->GetLightInfo()->param2.y
			, iter->second->GetLightInfo()->param2.z
			, iter->second->GetLightInfo()->param2.w);
	}
}