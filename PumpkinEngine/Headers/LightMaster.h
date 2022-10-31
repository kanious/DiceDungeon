#ifndef _LIGHTMASTER_H_
#define _LIGHTMASTER_H_

#include "Base.h"
#include "Light.h"

NAMESPACE_BEGIN(Engine)

class ENGINE_API CLightMaster : public CBase
{
	SINGLETON(CLightMaster)

private:
	typedef std::unordered_map<std::string, CLight*>	LIGHT_MAP;
	LIGHT_MAP						m_mapLights;

private:
	explicit CLightMaster();
	virtual ~CLightMaster();
	void Destroy();

public:
	LIGHT_MAP* GetLightMap()			{ return &m_mapLights; }
public:
	RESULT AddLight(std::string tag, CLight::cLightInfo* pInfo);
	RESULT RemoveLight(std::string tag);
	void SetUniformLocation(_uint shaderID);
	void SetLightInfo();
};

NAMESPACE_END

#endif //_LIGHTMASTER_H_