#ifndef _RIGIDBODYDESC_H_
#define _RIGIDBODYDESC_H_

#include "EngineDefines.h"
#include "glm\vec3.hpp"
#include "glm\gtx\quaternion.hpp"

NAMESPACE_BEGIN(Engine)

// Rigid Body description class
class ENGINE_API CRigidBodyDesc
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 orientation;
	glm::vec3 halfSize;
	glm::dvec3 forceAccum;
};

NAMESPACE_END

#endif //_RIGIDBODYDESC_H_