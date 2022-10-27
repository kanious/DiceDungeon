#ifndef _COMPONENTMANAGER_H_
#define _COMPONENTMANAGER_H_

#include "Define.h"
#include "OpenGLDefines.h"

namespace Engine
{
	class CComponent;
}

class ComponentManager
{
	SINGLETON(ComponentManager)

private:
	unordered_map<eMESH, Engine::CComponent*>			m_mapMesh;
	unordered_map<eSHADER, Engine::CComponent*>			m_mapShader;
	unordered_map<eTEXTURE, Engine::CComponent*>		m_mapTexture;
	Engine::CComponent*									m_baseTransform;

private:
	explicit ComponentManager();
	~ComponentManager();
public:
	void Destroy();

public:
	RESULT AddMesh(eMESH, Engine::CComponent*);
	RESULT AddShader(eSHADER, Engine::CComponent*);
	RESULT AddTexture(eTEXTURE, Engine::CComponent*);
	RESULT AddTransform(Engine::CComponent*);
	Engine::CComponent* CloneMesh(eMESH);
	Engine::CComponent* CloneShader(eSHADER);
	Engine::CComponent* CloneTexture(eTEXTURE);
	Engine::CComponent* CloneTransform();
};

#endif //_COMPONENTMANAGER_H_