#ifndef _COLLISIONMASTER_H_
#define _COLLISIONMASTER_H_

#include "Base.h"
#include "glm/vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CAABB;
class CTransform;

class ENGINE_API CCollisionMaster : public CBase
{
	SINGLETON(CCollisionMaster)

private:
	explicit CCollisionMaster();
	virtual ~CCollisionMaster();
	void Destroy();

public:
	_bool IntersectRayToVirtualPlane(_float planeSize, glm::vec3& vOrigin, glm::vec3& vDir, glm::vec3& vDest);
	_bool IntersectRayToBoundingBox(CAABB* pBoundingBox, CTransform* pParentTransform, glm::vec3& vOrigin, glm::vec3& vDir);
	_bool IntersectPointToTriangle(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& vOrigin, glm::vec3& vDir, glm::vec3& vDest);

private:
	_bool PointInTriangle(glm::vec3 P, glm::vec3 A, glm::vec3 B, glm::vec3 C);
	_bool PointInDotThree(glm::vec3 p1, glm::vec3 p2, glm::vec3 pA, glm::vec3 pB);
};

NAMESPACE_END

#endif //_COLLISIONMASTER_H_