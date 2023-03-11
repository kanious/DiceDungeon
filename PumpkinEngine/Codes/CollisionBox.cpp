#include "pch.h"
#include "..\Headers\CollisionBox.h"
#include "..\Headers\RigidBody.h"
#include "..\Headers\OpenGLDefines.h"

USING(Engine)
USING(std)
USING(glm)

CCollisionBox*	CCollisionBox::boxData[256] = {};
_uint			CCollisionBox::boxCount = 0;
CCollisionBox::CCollisionBox(CRigidBody* pBody)
{
	m_eShape = pBody->eShape;
	body = pBody;
	transform = transpose(body->transformMatrix) * offset;
	halfSize = vec3(body->halfSize.x, body->halfSize.y, body->halfSize.z);
	boxID = CCollisionBox::boxCount;
	CCollisionBox::boxData[boxID] = this;
	CCollisionBox::boxCount++;
}

CCollisionBox::~CCollisionBox()
{
}

//Returns the value of the axis corresponding to the index
vec3 CCollisionBox::GetAxis(_uint index) const
{
	return vec3(transform[0][index], transform[1][index], transform[2][index]);
}

// Return transform
const mat4& CCollisionBox::GetTransform() const
{
	return transform;
}

// Return modelMatrix;
mat4 CCollisionBox::GetModelMatrix()
{
	return CCollisionBox::boxData[boxID]->modelMatrix;
}

// Set modelMatrix (to rigidbody also)
void CCollisionBox::SetModelMatrix(mat4 m)
{
	CCollisionBox::boxData[boxID]->modelMatrix = m;
	CCollisionBox::boxData[boxID]->body->SetModelMatrix(m);
}

// Calculate transform
void CCollisionBox::CalculateInternals()
{
	transform = transpose(body->transformMatrix) * offset;
}
