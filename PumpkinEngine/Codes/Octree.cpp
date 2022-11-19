#include "pch.h"
#include "..\Headers\Octree.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\CollisionMaster.h"
#include "..\Headers\BoundingBox.h"
#include "..\Headers\Transform.h"


USING(Engine)
USING(glm)
USING(std)

COctree::COctree()
	: m_pParentTransform(nullptr)
{
	m_vecNodeInfo.clear();
}

COctree::~COctree()
{
}

void COctree::Destroy()
{
	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
	{
		for (int j = 0; j < m_vecNodeInfo[i]->vecTriangles.size(); ++j)
		{
			delete m_vecNodeInfo[i]->vecTriangles[j];
		}
		m_vecNodeInfo[i]->vecTriangles.clear();
		SafeDestroy(m_vecNodeInfo[i]->BBox);
		SafeDestroy(m_vecNodeInfo[i]->BBox_fixed);
		delete m_vecNodeInfo[i];
	}
	m_vecNodeInfo.clear();
}

void COctree::Render()
{
	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
	{
		if (m_vecNodeInfo[i]->BBox_Render && m_vecNodeInfo[i]->vecTriangles.size() > 0)
		{
			m_vecNodeInfo[i]->BBox->RenderWithoutParent();
		}
	}
}

void COctree::AddTriangle(TRIANGLE t, _uint& count)
{
	_bool flag = false;
	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
	{
		if (!m_vecNodeInfo[i]->isLeaf)
			continue;

		//if (CCollisionMaster::GetInstance()->IntersectTriangleToBoundingBox(&t, m_vecNodeInfo[i]->AABB))
		if (CCollisionMaster::GetInstance()->IntersectTriangleToOBB(&t, m_vecNodeInfo[i]->BBox))
		{
			TRIANGLE* tri = new TRIANGLE();
			tri->p0 = t.p0;
			tri->p1 = t.p1;
			tri->p2 = t.p2;
			m_vecNodeInfo[i]->vecTriangles.push_back(tri);
			flag = true;
		}
	}

	if (!flag)
	{
		cout << "Missed: " << t.p0.x << ", " << t.p0.y << ", " << t.p0.z << " \t\t ";
		cout << t.p1.x << ", " << t.p1.y << ", " << t.p1.z << " \t\t ";
		cout << t.p2.x << ", " << t.p2.y << ", " << t.p2.z << endl;
		++count;
	}
}

//void COctree::UpdateAABB(CTransform* parent)
//{
//	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
//	{
//		CBoundingBox* aabb = m_vecNodeInfo[i]->BBBox;
//		CBoundingBox* aabb_fixed = m_vecNodeInfo[i]->BBBox_fixed;
//
//		vec3 vParentPos = vParentPos = parent->GetPosition();
//		_float fAngleY = parent->GetRotationY();
//		vec3 vParentScale = parent->GetScale();
//		fAngleY = radians(fAngleY);
//
//		aabb_fixed->m_vMax = aabb->m_vMax * vParentScale;
//		_float x = aabb_fixed->m_vMax.x;
//		_float z = aabb_fixed->m_vMax.z;
//		aabb_fixed->m_vMax.x = (z * sin(fAngleY)) + (x * cos(fAngleY));
//		aabb_fixed->m_vMax.z = (z * cos(fAngleY)) - (x * sin(fAngleY));
//		aabb_fixed->m_vMax += vParentPos;
//
//		aabb_fixed->m_vMin = aabb->m_vMin * vParentScale;
//		x = aabb_fixed->m_vMin.x;
//		z = aabb_fixed->m_vMin.z;
//		aabb_fixed->m_vMin.x = (z * sin(fAngleY)) + (x * cos(fAngleY));
//		aabb_fixed->m_vMin.z = (z * cos(fAngleY)) - (x * sin(fAngleY));
//		aabb_fixed->m_vMin += vParentPos;
//
//		aabb_fixed->m_vHalfExtents = (aabb_fixed->m_vMax - aabb_fixed->m_vMin) / 2.f;
//		aabb_fixed->m_vCenter = aabb_fixed->m_vMin + aabb_fixed->m_vHalfExtents;
//	}
//}

void COctree::CheckBoundingBox(CBoundingBox* bbox, vector<COctreeNode*>& vecNode)
{
	if (nullptr == bbox)
		return;

	//for (int i = 0; i < m_vecNodeInfo.size(); ++i)
	//	m_vecNodeInfo[i]->BBox_Render = false;
	
	CheckNodeBoundingBox(0, bbox, vecNode);
}

void COctree::CheckNodeBoundingBox(_int index, CBoundingBox* bbox, vector<COctreeNode*>& vecNode)
{
	COctreeNode* pNode = m_vecNodeInfo[index];
	CBoundingBox* nodeBBBox = pNode->BBox_fixed;

	if (pNode->isLeaf && pNode->vecTriangles.size() == 0)
		return;

	//if (0 != CCollisionMaster::GetInstance()->IntersectAABBToAABB(aabb, nodeAABB))
	if (0 != CCollisionMaster::GetInstance()->IntersectOBBToOBB(bbox, nodeBBBox))
	{
		if (pNode->isLeaf)
		{
			pNode->BBox_Render = true;
			vecNode.push_back(pNode);
		}
		else
		{
			CheckNodeBoundingBox(pNode->childIndex[0], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[1], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[2], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[3], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[4], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[5], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[6], bbox, vecNode);
			CheckNodeBoundingBox(pNode->childIndex[7], bbox, vecNode);
		}
	}
}

//_int COctree::CheckAABBToTriangle(CAABB* tri)
//{
//	if (nullptr == tri)
//		return false;
//
//	//for (int i = 0; i < m_vecNodeInfo.size(); ++i)
//	//	m_vecNodeInfo[i]->AABB_Render = false;
//
//	_int result = CheckNodeAABBToTriangle(0, tri);
//	//_int result = CCollisionMaster::GetInstance()->IntersectAABBToAABB(tri, nodeAABB);
//
//	return result;
//}
//
//_int COctree::CheckNodeAABBToTriangle(_int index, CAABB* tri)
//{
//	COctreeNode* pNode = m_vecNodeInfo[index];
//	CAABB* nodeAABB = pNode->AABB_fixed;
//
//	if (pNode->isLeaf && pNode->vecTriangles.size() == 0)
//		return false;
//
//	_int result = CCollisionMaster::GetInstance()->IntersectAABBToAABB(tri, nodeAABB);
//	if (result != 0)
//	{
//		if (pNode->isLeaf)
//		{
//			pNode->AABB_Render = true;
//			return result;
//		}
//		else
//		{
//			_int childResult = 0;
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[0], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[1], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[2], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[3], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[4], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[5], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[6], tri);
//			if (0 != childResult)
//				return childResult;
//
//			childResult = CheckNodeAABBToTriangle(pNode->childIndex[7], tri);
//			if (0 != childResult)
//				return childResult;
//
//		}
//	}
//	return result;
//}
//
//_int COctree::GetHashValue(_float x, _float y, _float z)
//{
//	_float xValue = floor(x);
//	xValue = floor(xValue + m_vHashRange.x);
//	xValue = floor(xValue / m_vHashRange.x);
//	xValue *= 1000000;
//
//	_float yValue = floor(y);
//	yValue = floor(yValue + m_vHashRange.y);
//	yValue = floor(yValue / m_vHashRange.y);
//	yValue *= 1000;
//
//	_float zValue = floor(z);
//	zValue = floor(zValue + m_vHashRange.z);
//	zValue = floor(zValue / m_vHashRange.z);
//
//	return (_int)xValue + (_int)yValue + (_int)zValue;
//}
//
//void COctree::AddTriangleToTreeNode(_int hashKey, TRIANGLE triangle, _uint& index)
//{
//	MAP_NODE::iterator iter = m_mapNodes.find(hashKey);
//	if (iter != m_mapNodes.end())
//	{
//		TRIANGLE* newTriangle = new TRIANGLE();
//		newTriangle->p0 = triangle.p0;
//		newTriangle->p1 = triangle.p1;
//		newTriangle->p2 = triangle.p2;
//		iter->second->vecTriangles.push_back(newTriangle);
//	}
//	else
//		++index;
//}
//
//void COctree::CheckTriangles()
//{
//	int count = 0;
//	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
//	{
//		if (m_vecNodeInfo[i]->vecTriangles.size() > 0)
//		{
//			cout << "index " << m_vecNodeInfo[i]->myIndex;
//			cout << " : " << m_vecNodeInfo[i]->vecTriangles.size() << " triangles." << endl;
//			count++;
//		}
//	}
//
//	cout << "총 " << m_vecNodeInfo.size() << "개의 노드 중 " << count << "개의 노드가 삼각형 보유" << endl;
//}
//
//
//void COctree::GetLeafNodes(vec3 vMain, vec3 vTarget, vector<COctreeNode*>& vecNode)
//{
//	vec3 vMax(0.f);
//	vec3 vMin(0.f);
//
//	if (vMain.x > vTarget.x) { vMax.x = vMain.x; vMin.x = vTarget.x; }
//	else					 { vMax.x = vTarget.x; vMin.x = vMain.x; }
//
//	if (vMain.y > vTarget.y) { vMax.y = vMain.y; vMin.y = vTarget.y; }
//	else					 { vMax.y = vTarget.y; vMin.y = vMain.y; }
//
//	if (vMain.z > vTarget.z) { vMax.z = vMain.z; vMin.z = vTarget.z; }
//	else					 { vMax.z = vTarget.z; vMin.z = vMain.z; }
//
//	vec3 vHalf = (vMax - vMin) / 2.f;
//	vec3 vCenter = vMin + vHalf;
//	_float fRadius = distance(vCenter, vMax);
//
//	vec3 vDir = vTarget - vMain;
//	vDir = normalize(vDir);
//
//	for (int i = 0; i < m_vecNodeInfo.size(); ++i)
//	{
//		m_vecNodeInfo[i]->AABB_Render = false;
//	}
//
//	CheckDistanceOctree(0, vCenter, fRadius, vMain, vDir, vecNode);
//}
//
//void COctree::CheckDistanceOctree(_int index, vec3 vCenter, _float fRadius
//	, vec3 vMain, vec3 vDir, vector<COctreeNode*>& vecNode)
//{
//	COctreeNode* pNode = m_vecNodeInfo[index];
//	_float fCenterDist = distance(vCenter, pNode->vCenter);
//
//	if (fCenterDist <= fRadius + pNode->fRadius + 5.0f)
//	{
//		_bool check = CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(pNode->AABB, nullptr, vMain, vDir);
//		if (check)
//		{
//			if (pNode->isLeaf)
//			{
// 				pNode->AABB_Render = true;
//				vecNode.push_back(pNode);
//			}
//			else
//			{
//				CheckDistanceOctree(pNode->childIndex[0], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[1], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[2], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[3], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[4], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[5], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[6], vCenter, fRadius, vMain, vDir, vecNode);
//				CheckDistanceOctree(pNode->childIndex[7], vCenter, fRadius, vMain, vDir, vecNode);
//			}
//		}
//	}
//
//}

_bool Compare(COctree::COctreeNode* t1, COctree::COctreeNode* t2)
{
	return t1->myIndex < t2->myIndex;
}

RESULT COctree::Ready(vec3 vMax, vec3 vMin, _uint depth)
{
	_uint nodeCount = (pow(8, depth) - 1) / 7;
	m_vecNodeInfo.resize(nodeCount);

	vec3 vHalf = (vMax - vMin) / 2.f;
	ReadyOctree(0, vMin + vHalf, vHalf, 1, depth);
	sort(m_vecNodeInfo.begin(), m_vecNodeInfo.end(), Compare);

	return PK_NOERROR;
}

void COctree::ReadyOctree(_uint index, glm::vec3 vCenter, vec3 vHalf, _uint depth, _uint maxDepth)
{
	COctreeNode* pNode = new COctreeNode();
	pNode->myIndex = index;
	pNode->vMin = vCenter - vHalf;
	pNode->vMax = vCenter + vHalf;
	pNode->vHalf = vHalf;
	pNode->vCenter = vCenter;
	pNode->fRadius = distance(pNode->vCenter, pNode->vMax);
	pNode->BBox = CBoundingBox::Create(pNode->vMin, pNode->vMax, "DebugBoxShader");
	pNode->BBox_fixed = CBoundingBox::Create(pNode->vMin, pNode->vMax, "DebugBoxShader");

	if (depth == maxDepth)
	{
		pNode->isLeaf = true;
		pNode->BBox_Render = true;
	}
	else
	{
		pNode->isLeaf = false;
		pNode->BBox_Render = false;

		pNode->childIndex[0] = 1 + (8 * index);
		pNode->childIndex[1] = 2 + (8 * index);
		pNode->childIndex[2] = 3 + (8 * index);
		pNode->childIndex[3] = 4 + (8 * index);
		pNode->childIndex[4] = 5 + (8 * index);
		pNode->childIndex[5] = 6 + (8 * index);
		pNode->childIndex[6] = 7 + (8 * index);
		pNode->childIndex[7] = 8 + (8 * index);

		vec3 childHalf = vHalf / 2.f;
		vec3 childCenter = vec3(0.f);
		childCenter.x = pNode->vCenter.x - childHalf.x;
		childCenter.y = pNode->vCenter.y - childHalf.y;
		childCenter.z = pNode->vCenter.z + childHalf.z;
		ReadyOctree(pNode->childIndex[0], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x + childHalf.x;
		childCenter.y = pNode->vCenter.y - childHalf.y;
		childCenter.z = pNode->vCenter.z + childHalf.z;
		ReadyOctree(pNode->childIndex[1], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x - childHalf.x;
		childCenter.y = pNode->vCenter.y - childHalf.y;
		childCenter.z = pNode->vCenter.z - childHalf.z;
		ReadyOctree(pNode->childIndex[2], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x + childHalf.x;
		childCenter.y = pNode->vCenter.y - childHalf.y;
		childCenter.z = pNode->vCenter.z - childHalf.z;
		ReadyOctree(pNode->childIndex[3], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x - childHalf.x;
		childCenter.y = pNode->vCenter.y + childHalf.y;
		childCenter.z = pNode->vCenter.z + childHalf.z;
		ReadyOctree(pNode->childIndex[4], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x + childHalf.x;
		childCenter.y = pNode->vCenter.y + childHalf.y;
		childCenter.z = pNode->vCenter.z + childHalf.z;
		ReadyOctree(pNode->childIndex[5], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x - childHalf.x;
		childCenter.y = pNode->vCenter.y + childHalf.y;
		childCenter.z = pNode->vCenter.z - childHalf.z;
		ReadyOctree(pNode->childIndex[6], childCenter, childHalf, depth + 1, maxDepth);

		childCenter.x = pNode->vCenter.x + childHalf.x;
		childCenter.y = pNode->vCenter.y + childHalf.y;
		childCenter.z = pNode->vCenter.z - childHalf.z;
		ReadyOctree(pNode->childIndex[7], childCenter, childHalf, depth + 1, maxDepth);
	}

	m_vecNodeInfo[pNode->myIndex] = pNode;
}

COctree* COctree::Create(vec3 vMax, vec3 vMin, _uint depth)
{
	COctree* pInstance = new COctree();
	if (PK_NOERROR != pInstance->Ready(vMax, vMin, depth))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
