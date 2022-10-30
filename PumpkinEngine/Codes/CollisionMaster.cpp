#include "pch.h"
#include "..\Headers\CollisionMaster.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\AABB.h"
#include "..\Headers\Transform.h"


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

_bool CCollisionMaster::IntersectRayToBoundingBox(CAABB* pBoundingBox, CTransform* pParentTransform, vec3& vOrigin, vec3& vDir)
{
	if (nullptr == pBoundingBox || nullptr == pParentTransform)
		return false;

	vec3 vParentPos = pParentTransform->GetPosition();
	_float fAngleY = pParentTransform->GetRotationY();
	vec3 vParentScale = pParentTransform->GetScale();
	fAngleY = radians(fAngleY);
	VTX* pVertices = pBoundingBox->GetVertices();

	vec3 point[8];
	ZeroMemory(point, sizeof(point));
	for (int i = 0; i < 8; ++i)
	{
		vec3 vPos = vec3(pVertices[i].vPos.x, pVertices[i].vPos.y, pVertices[i].vPos.z);
		point[i] = vPos * vParentScale;
		_float x = point[i].x;
		_float z = point[i].z;
		point[i].x = (z * sin(fAngleY)) + (x * cos(fAngleY)); //x = zsin(b) + xcos(b);
		point[i].z = (z * cos(fAngleY)) - (x * sin(fAngleY)); //z = zcos(b) - xsin(b);
		point[i] = point[i] + vParentPos;
	}

	vec3 vDest;
	if (IntersectPointToTriangle(point[0], point[1], point[2], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[0], point[2], point[3], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[0], point[4], point[5], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[0], point[5], point[1], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[2], point[6], point[7], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[2], point[7], point[3], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[1], point[5], point[6], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[1], point[6], point[2], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[3], point[7], point[4], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[3], point[4], point[0], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[5], point[4], point[7], vOrigin, vDir, vDest))return true;
	if (IntersectPointToTriangle(point[5], point[7], point[6], vOrigin, vDir, vDest))return true;

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