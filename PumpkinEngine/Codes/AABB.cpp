#include "pch.h"
#include "..\Headers\AABB.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\VIBuffer.h"
#include "..\Headers\Transform.h"


USING(Engine)
USING(glm)

CAABB::CAABB()
	: m_pVIBuffer(nullptr), m_pVertices(nullptr)
	, m_vCenter(vec3(0.f)), m_vHalfExtents(vec3(0.f))
	, m_vMin(vec3(0.f)), m_vMax(vec3(0.f)), m_vMinWorld(vec3(0.f)), m_vMaxWorld(vec3(0.f))
{
}

CAABB::CAABB(const CAABB& rhs)
	: m_pVIBuffer(rhs.m_pVIBuffer), m_pVertices(rhs.m_pVertices)
	, m_vCenter(rhs.m_vCenter), m_vHalfExtents(rhs.m_vHalfExtents)
	, m_vMin(rhs.m_vMin), m_vMax(rhs.m_vMax), m_vMinWorld(rhs.m_vMinWorld), m_vMaxWorld(rhs.m_vMaxWorld)
{
	if (nullptr != m_pVIBuffer) m_pVIBuffer->AddRefCnt();
}

CAABB::~CAABB()
{
}

void CAABB::Render()
{
 	if (nullptr != m_pVIBuffer)
		m_pVIBuffer->Render();
}

void CAABB::Destroy()
{
	delete[] m_pVertices;

	SafeDestroy(m_pVIBuffer);
}

RESULT CAABB::Ready(vec3 center, vec3 half, vec3 min, vec3 max)
{
	m_tag = "AABB";

	m_vCenter = vec3(center.x, center.y, center.z);
	m_vHalfExtents = vec3(half.x, half.y, half.z);
	m_vMin = vec3(min.x, min.y, min.z);
	m_vMax = vec3(max.x, max.y, max.z);
	m_vMinWorld = vec3(min.x, min.y, min.z);
	m_vMaxWorld = vec3(max.x, max.y, max.z);
	
	// Buffer
    _uint vertexNum = 8;
    _uint indexNum = 36;

	m_pVertices = new VTX[vertexNum];
    memset(m_pVertices, 0, sizeof(*m_pVertices));
	m_pVertices[0].vPos = vec3(min.x, max.y, max.z);
	m_pVertices[1].vPos = vec3(min.x, max.y, min.z);
	m_pVertices[2].vPos = vec3(max.x, max.y, min.z);
	m_pVertices[3].vPos = vec3(max.x, max.y, max.z);

	m_pVertices[4].vPos = vec3(min.x, min.y, max.z);
	m_pVertices[5].vPos = vec3(min.x, min.y, min.z);
	m_pVertices[6].vPos = vec3(max.x, min.y, min.z);
	m_pVertices[7].vPos = vec3(max.x, min.y, max.z);

	_uint* pIndices = new _uint[indexNum]
	{
		0, 1, 2,
		0, 2, 3,
		0, 4, 5,
		0, 5 ,1,
		2, 6, 7,
		2, 7, 3,
		1, 5, 6,
		1, 6, 2,
		3, 7, 4,
		3, 4, 0,
		5, 4, 7,
		5, 7, 6
	};

	m_pVIBuffer = CVIBuffer::Create(vertexNum, m_pVertices, indexNum, pIndices, xyz_index);
	if (nullptr != m_pVIBuffer)
		m_pVIBuffer->SetWireFrame(true);

	delete[] pIndices;

	return PK_NOERROR;
}

CComponent* CAABB::Clone()
{
	return new CAABB(*this);
}

CAABB* CAABB::Create(vec3 center, vec3 half, vec3 min, vec3 max)
{
	CAABB* pInstance = new CAABB;
	if (PK_NOERROR != pInstance->Ready(center, half, min, max))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
