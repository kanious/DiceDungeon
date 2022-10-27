#include "pch.h"
#include "..\Headers\CollisionMaster.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\AABB.h"


USING(Engine)
USING(glm)
SINGLETON_FUNCTION(CCollisionMaster)

CCollisionMaster::CCollisionMaster()
{
}

CCollisionMaster::~CCollisionMaster()
{
}

void CCollisionMaster::Destroy()
{
}

_bool CCollisionMaster::IntersectRayToVirtualPlane(_float planeSize, vec3& vOrigin, vec3& vDir, vec3& vDest)
{
	vec3 p0 = vec3(-planeSize, 0.f, -planeSize);
	vec3 p1 = vec3(-planeSize, 0.f, planeSize);
	vec3 p2 = vec3(planeSize, 0.f, planeSize);
	vec3 p3 = vec3(planeSize, 0.f, -planeSize);

	if (IntersectPointToTriangle(p0, p1, p2, vOrigin, vDir, vDest))
		return true;

	if (IntersectPointToTriangle(p0, p2, p3, vOrigin, vDir, vDest))
		return true;

	return false;
}

_bool CCollisionMaster::IntersectRayToBoundingBox(CAABB* pBoundingBox, const vec3& vParentPos, const vec3& vParentScale, vec3& vOrigin, vec3& vDir)
{
	if (nullptr == pBoundingBox)
		return false;

	VTX* pVertices = pBoundingBox->GetVertices();

	vec3 p0 = (pVertices[0].vPos * vParentScale) + vParentPos;
	vec3 p1 = (pVertices[1].vPos * vParentScale) + vParentPos;
	vec3 p2 = (pVertices[2].vPos * vParentScale) + vParentPos;
	vec3 p3 = (pVertices[3].vPos * vParentScale) + vParentPos;
	vec3 p4 = (pVertices[4].vPos * vParentScale) + vParentPos;
	vec3 p5 = (pVertices[5].vPos * vParentScale) + vParentPos;
	vec3 p6 = (pVertices[6].vPos * vParentScale) + vParentPos;
	vec3 p7 = (pVertices[7].vPos * vParentScale) + vParentPos;

	vec3 vDest;
	if (IntersectPointToTriangle(p0, p1, p2, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p0, p2, p3, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p0, p4, p5, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p0, p5, p1, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p2, p6, p7, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p2, p7, p3, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p1, p5, p6, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p1, p6, p2, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p3, p7, p4, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p3, p4, p0, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p5, p4, p7, vOrigin, vDir, vDest))
		return true;
	if (IntersectPointToTriangle(p5, p7, p6, vOrigin, vDir, vDest))
		return true;

	return false;
}

_bool CCollisionMaster::IntersectPointToTriangle(vec3& p0, vec3& p1, vec3& p2, vec3& vOrigin, vec3& vDir, vec3& vDest)
{
	vec3 vNormal = cross(p2 - p0, p1 - p0);
	_float n = dot(p0 - vOrigin, vNormal);
	_float d = n / dot(vDir, vNormal);
	vDest = vOrigin + vDir * d;

	return PointInTriangle(vDest, p0, p1, p2);
}

_bool CCollisionMaster::PointInTriangle(vec3 P, vec3 A, vec3 B, vec3 C)
{
	return PointInDotThree(P, A, B, C) && PointInDotThree(P, B, C, A) && PointInDotThree(P, C, A, B);
}

_bool CCollisionMaster::PointInDotThree(vec3 p1, vec3 p2, vec3 pA, vec3 pB)
{
	vec3 cp1 = cross(pB - pA, p1 - pA);
	vec3 cp2 = cross(pB - pA, p2 - pA);
	return dot(cp1, cp2) >= 0;
}