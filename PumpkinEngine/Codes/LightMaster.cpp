#include "pch.h"
#include "..\Headers\LightMaster.h"
#include "..\Headers\Light.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\XMLParser.h"
#include <sstream>

USING(Engine)
USING(std)
SINGLETON_FUNCTION(CLightMaster)

CLightMaster::CLightMaster()
	: m_ShaderProgram(0)
{
	m_vecLights.clear();
}

CLightMaster::~CLightMaster()
{
}

void CLightMaster::Destroy()
{
	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
		SafeDestroy(*iter);
	m_vecLights.clear();
}

RESULT CLightMaster::AddLight(CLight::cLightInfo* pInfo)
{
	if (20 <= m_vecLights.size())
		return PK_ERROR;

	CLight* newLight = CLight::Create(pInfo);
	newLight->SetShaderProgram(m_ShaderProgram);
	m_vecLights.push_back(newLight);

	SetUniformLocation();

	return PK_NOERROR;
}

RESULT CLightMaster::RemoveLight(CLight* pLight)
{
	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
	{
		if (*iter == pLight)
		{
			(*iter)->ResetLightInfo();
			m_vecLights.erase(iter);
			break;
		}
	}
	return PK_NOERROR;
}

void CLightMaster::SetShader(_uint shaderID)
{
	m_ShaderProgram = shaderID;
}

void CLightMaster::SetUniformLocation()
{
	_int index = 0;
	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
	{
		stringstream ss;
		ss << "theLights[" << index << "].position";
		(*iter)->SetLocation(0, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].direction";
		(*iter)->SetLocation(1, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].diffuse";
		(*iter)->SetLocation(2, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].specular";
		(*iter)->SetLocation(3, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].ambient";
		(*iter)->SetLocation(4, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].atten";
		(*iter)->SetLocation(5, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].param1";
		(*iter)->SetLocation(6, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		ss.str("");
		ss << "theLights[" << index << "].param2";
		(*iter)->SetLocation(7, glGetUniformLocation(m_ShaderProgram, ss.str().c_str()));

		++index;
	}
}

void CLightMaster::SetLightInfo()
{
	glUseProgram(m_ShaderProgram);

	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
	{
		glUniform4f((*iter)->GetLocation(0)
			, (*iter)->GetLightInfo()->position.x
			, (*iter)->GetLightInfo()->position.y
			, (*iter)->GetLightInfo()->position.z
			, (*iter)->GetLightInfo()->position.w);

		glUniform4f((*iter)->GetLocation(1)
			, (*iter)->GetLightInfo()->direction.x
			, (*iter)->GetLightInfo()->direction.y
			, (*iter)->GetLightInfo()->direction.z
			, (*iter)->GetLightInfo()->direction.w);

		glUniform4f((*iter)->GetLocation(2)
			, (*iter)->GetLightInfo()->diffuse.x
			, (*iter)->GetLightInfo()->diffuse.y
			, (*iter)->GetLightInfo()->diffuse.z
			, (*iter)->GetLightInfo()->diffuse.w);

		glUniform4f((*iter)->GetLocation(3)
			, (*iter)->GetLightInfo()->specular.x
			, (*iter)->GetLightInfo()->specular.y
			, (*iter)->GetLightInfo()->specular.z
			, (*iter)->GetLightInfo()->specular.w);

		glUniform4f((*iter)->GetLocation(4)
			, (*iter)->GetLightInfo()->ambient.x
			, (*iter)->GetLightInfo()->ambient.y
			, (*iter)->GetLightInfo()->ambient.z
			, (*iter)->GetLightInfo()->ambient.w);

		glUniform4f((*iter)->GetLocation(5)
			, (*iter)->GetLightInfo()->atten.x
			, (*iter)->GetLightInfo()->atten.y
			, (*iter)->GetLightInfo()->atten.z
			, (*iter)->GetLightInfo()->atten.w);

		glUniform4f((*iter)->GetLocation(6)
			, (*iter)->GetLightInfo()->param1.x
			, (*iter)->GetLightInfo()->param1.y
			, (*iter)->GetLightInfo()->param1.z
			, (*iter)->GetLightInfo()->param1.w);

		glUniform4f((*iter)->GetLocation(7)
			, (*iter)->GetLightInfo()->param2.x
			, (*iter)->GetLightInfo()->param2.y
			, (*iter)->GetLightInfo()->param2.z
			, (*iter)->GetLightInfo()->param2.w);
	}
}
void CLightMaster::SaveLights(string path)
{
	vector<CXMLParser::sLightData> vecLights;

	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
	{
		CLight::cLightInfo* pInfo = (*iter)->GetLightInfo();

		CXMLParser::sLightData data;
		data.NAME = pInfo->name;
		data.POSITION = pInfo->position;
		data.DIRECTION = pInfo->direction;
		data.DIFFUSE = pInfo->diffuse;
		data.SPECULAR = pInfo->specular;
		data.AMBIENT = pInfo->ambient;
		data.ATTEN = pInfo->atten;
		data.PARAM1 = pInfo->param1;
		data.PARAM2 = pInfo->param2;

		vecLights.push_back(data);
	}
	CXMLParser::GetInstance()->SaveLightData(path, vecLights);
}

void CLightMaster::LoadLights(string path)
{
	vector<CLight*>::iterator iter;
	for (iter = m_vecLights.begin(); iter != m_vecLights.end(); ++iter)
		SafeDestroy(*iter);
	m_vecLights.clear();

	vector<CXMLParser::sLightData> vecLights;
	CXMLParser::GetInstance()->LoadLightData(path, vecLights);
	vector<CXMLParser::sLightData>::iterator iterLight;
	for (iterLight = vecLights.begin(); iterLight != vecLights.end(); ++iterLight)
	{
		CLight::cLightInfo* pInfo = new CLight::cLightInfo();
		pInfo->name = iterLight->NAME;
		pInfo->position = iterLight->POSITION;
		pInfo->direction = iterLight->DIRECTION;
		pInfo->diffuse = iterLight->DIFFUSE;
		pInfo->specular = iterLight->SPECULAR;
		pInfo->ambient = iterLight->AMBIENT;
		pInfo->atten = iterLight->ATTEN;
		pInfo->param1 = iterLight->PARAM1;
		pInfo->param2 = iterLight->PARAM2;

		AddLight(pInfo);
	}

	SetUniformLocation();
}