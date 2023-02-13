#ifndef _OBJECTFACTORY_H_
#define _OBJECTFACTORY_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CLayer;
};

class BGObject;
class Player;
class DefaultCamera;

class ObjectFactory
{
public:
	static BGObject* CreateBGObject(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);

	static Player* CreatePlayer(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer, std::string meshID,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale);

	static DefaultCamera* CreateCamera(_uint sTag, _uint lTag, _uint oTag, Engine::CLayer* pLayer,
		glm::vec3 vPos, glm::vec3 vRot, glm::vec3 vScale, _float fov, _float fNear, _float fFar);
};

#endif //_OBJECTFACTORY_H_