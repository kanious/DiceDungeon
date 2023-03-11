#include "pch.h"
#include "..\Headers\CollisionDetector.h"
#include "..\Headers\CollisionBox.h"
#include "..\Headers\CollisionData.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\Contact.h"
#include <glm/gtx/norm.hpp>

USING(Engine)
USING(std)
USING(glm)

#define CHECK_OVERLAP(axis, index) \
    if (!TryAxis(one, two, (axis), toCentre, (index), pen, best)) return 0;

// Check if two objects collide and add Contact
_uint CCollisionDetector::BoxAndBox(const CCollisionBox& one, const CCollisionBox& two, CCollisionData* data)
{
	vec3 toCentre = two.GetAxis(3) - one.GetAxis(3);

	_float pen = FLT_MAX;
	_uint best = 0xffffff;

	CHECK_OVERLAP(one.GetAxis(0), 0);
	CHECK_OVERLAP(one.GetAxis(1), 1);
	CHECK_OVERLAP(one.GetAxis(2), 2);

	CHECK_OVERLAP(two.GetAxis(0), 3);
	CHECK_OVERLAP(two.GetAxis(1), 4);
	CHECK_OVERLAP(two.GetAxis(2), 5);

	_uint bestSingleAxis = best;

	CHECK_OVERLAP(cross(one.GetAxis(0), two.GetAxis(0)), 6);
	CHECK_OVERLAP(cross(one.GetAxis(0), two.GetAxis(1)), 7);
	CHECK_OVERLAP(cross(one.GetAxis(0), two.GetAxis(2)), 8);
	CHECK_OVERLAP(cross(one.GetAxis(1), two.GetAxis(0)), 9);
	CHECK_OVERLAP(cross(one.GetAxis(1), two.GetAxis(1)), 10);
	CHECK_OVERLAP(cross(one.GetAxis(1), two.GetAxis(2)), 11);
	CHECK_OVERLAP(cross(one.GetAxis(2), two.GetAxis(0)), 12);
	CHECK_OVERLAP(cross(one.GetAxis(2), two.GetAxis(1)), 13);
	CHECK_OVERLAP(cross(one.GetAxis(2), two.GetAxis(2)), 14);

	assert(best != 0xffffff);

	if (best < 3)
	{
		FillPointFaceBoxBox(one, two, toCentre, data, best, pen);
		data->AddContacts(1);
		return 1;
	}
	else if (best < 6)
	{
		FillPointFaceBoxBox(two, one, toCentre * -1.f, data, best - 3, pen);
		data->AddContacts(1);
		return 1;
	}
	else
	{
		best -= 6;
		_uint oneAxisIndex = best / 3;
		_uint twoAxisIndex = best % 3;
		vec3 oneAxis = one.GetAxis(oneAxisIndex);
		vec3 twoAxis = two.GetAxis(twoAxisIndex);
		vec3 axis = cross(oneAxis, twoAxis);
		axis = normalize(axis);

		if (dot(axis, toCentre) > 0)
			axis *= -1.f;

		vec3 ptOnOneEdge = one.halfSize;
		vec3 ptOnTwoEdge = two.halfSize;
		for (unsigned int i = 0; i < 3; ++i)
		{
			if (i == oneAxisIndex)
				ptOnOneEdge[i] = 0;
			else if (0 < dot(one.GetAxis(i), axis))
				ptOnOneEdge[i] = -ptOnOneEdge[i];

			if (i == twoAxisIndex)
				ptOnTwoEdge[i] = 0;
			else if (0 > dot(two.GetAxis(i), axis))
				ptOnTwoEdge[i] = -ptOnTwoEdge[i];
		}

		vec4 temp = vec4(ptOnOneEdge.x, ptOnOneEdge.y, ptOnOneEdge.z, 1.f) * one.transform;
		vec3 v(temp.x, temp.y, temp.z);

		ptOnOneEdge = v;

		temp = vec4(ptOnTwoEdge.x, ptOnTwoEdge.y, ptOnTwoEdge.z, 1.f) * two.transform;
		v = vec3(temp.x, temp.y, temp.z);

		ptOnTwoEdge = v;

		vec3 vertex = ContactPoint(
			ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
			ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
			bestSingleAxis > 2);

		CContact* contact = data->contacts;

		contact->penetration = pen;
		contact->contactNormal = axis;
		contact->contactPoint = vertex;
		contact->SetBodyData(one.body, two.body, data->friction, data->restitution);
		data->AddContacts(1);

		return 1;
	}

	return 0;
}

// Check Axis
inline _bool CCollisionDetector::TryAxis(const CCollisionBox& one, const CCollisionBox& two, vec3 axis, const vec3& toCentre, _uint index, _float& smallestPenetration, _uint& smallestCase)
{
	if (0.0001f > length2(axis))
		return true;

	axis = normalize(axis);

	_float penetration = PenetrationOnAxis(one, two, axis, toCentre);

	if (penetration < 0)
		return false;

	if (penetration < smallestPenetration)
	{
		smallestPenetration = penetration;
		smallestCase = index;
	}

	return true;
}

// Check if two objects penetrate each other
_float CCollisionDetector::PenetrationOnAxis(const CCollisionBox& one, const CCollisionBox& two, const vec3& axis, const vec3& toCentre)
{
	_float oneProject = TransformToAxis(one, axis);
	_float twoProject = TransformToAxis(two, axis);

	_float dist = fabs(dot(toCentre, axis));

	return oneProject + twoProject - dist;
}

// Return project value
inline _float CCollisionDetector::TransformToAxis(const CCollisionBox& box, const vec3& axis)
{
	return box.halfSize.x * fabs(dot(axis, box.GetAxis(0))) +
		box.halfSize.y * fabs(dot(axis, box.GetAxis(1))) +
		box.halfSize.z * fabs(dot(axis, box.GetAxis(2)));

	return _float();
}

// Fill point
void CCollisionDetector::FillPointFaceBoxBox(const CCollisionBox& one, const CCollisionBox& two, const vec3& toCentre, CCollisionData* data, _uint best, _float pen)
{
	CContact* contact = data->contacts;

	vec3 normal = one.GetAxis(best);
	if (0 < dot(one.GetAxis(best), toCentre))
		normal *= -1.f;

	vec3 vertex = two.halfSize;
	if (0 > dot(two.GetAxis(0), normal))
		vertex.x *= -1.f;
	if (0 > dot(two.GetAxis(1), normal))
		vertex.y *= -1.f;
	if (0 > dot(two.GetAxis(2), normal))
		vertex.z *= -1.f;

	contact->contactNormal = normal;
	contact->penetration = pen;
	vec4 temp = vec4(vertex.x, vertex.y, vertex.z, 1.0) * two.GetTransform();
	vec3 v = vec3(temp.x, temp.y, temp.z);

	contact->contactPoint = v;
	contact->SetBodyData(one.body, two.body, data->friction, data->restitution);
}

// Return contact point between two objects
inline vec3 CCollisionDetector::ContactPoint(const vec3& pOne, const vec3& dOne, _float oneSize, const vec3& pTwo, const vec3& dTwo, _float twoSize, _bool useOne)
{
	vec3 toSt, cOne, cTwo;
	_float dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	_float denom, mua, mub;

	smOne = length2(dOne);
	smTwo = length2(dTwo);
	dpOneTwo = dot(dTwo, dOne);

	toSt = pOne - pTwo;
	dpStaOne = dot(dOne, toSt);
	dpStaTwo = dot(dTwo, toSt);

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	if (fabs(denom) < 0.0001f)
		return useOne ? pOne : pTwo;

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	if (mua > oneSize || mua < -oneSize || mub > twoSize || mub < -twoSize)
		return useOne ? pOne : pTwo;
	else
	{
		cOne = pOne + dOne * mua;
		cTwo = pTwo + dTwo * mub;

		return cOne * 0.5f + cTwo * 0.5f;
	}
}
