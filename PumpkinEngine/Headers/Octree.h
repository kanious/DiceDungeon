#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Base.h"
#include "EngineStruct.h"

NAMESPACE_BEGIN(Engine)

class CBoundingBox;
class CTransform;

class ENGINE_API COctree : public CBase
{
public:
	class COctreeNode
	{
	public:
		_bool					isLeaf;
		glm::vec3				vMin;
		glm::vec3				vMax;
		glm::vec3				vCenter;
		glm::vec3				vHalf;
		_float					fRadius;
		_uint					myIndex;
		_uint					childIndex[8];
		std::vector<TRIANGLE*>	vecTriangles;
		CBoundingBox*			BBox;
		CBoundingBox*			BBox_fixed;
		_bool					BBox_Render;
		explicit COctreeNode()
			: isLeaf(false), myIndex(0), fRadius(0.f), BBox(nullptr), BBox_fixed(nullptr), BBox_Render(false)
			, vMin(glm::vec3(0.f)), vMax(glm::vec3(0.f)), vCenter(glm::vec3(0.f)), vHalf(glm::vec3(0.f))
		{
			vecTriangles.clear();
			memset(childIndex, 0, sizeof(_uint) * 4);
		}
		~COctreeNode() {}
	};

private:
	std::vector<COctreeNode*>							m_vecNodeInfo;
	CTransform*											m_pParentTransform;

private:
	explicit COctree();
	virtual ~COctree();
	void Destroy();
public:
	void Render();

public:
	void SetParentTransform(CTransform* parent)		{ m_pParentTransform = parent; }
	std::vector<COctreeNode*> GetNodeVector()		{ return m_vecNodeInfo; }
	void AddTriangle(TRIANGLE t, _uint& count);
	//void UpdateAABB(CTransform* parent);
	void CheckBoundingBox(CBoundingBox* bbox, std::vector<COctreeNode*>& vecNode);
private:
	void CheckNodeBoundingBox(_int index, CBoundingBox* bbox, std::vector<COctreeNode*>& vecNode);

//public:
//	_int CheckAABBToTriangle(CBoundingBox* tri);
//private:
//	_int CheckNodeAABBToTriangle(_int index, CBoundingBox* tri);
	//_int GetHashValue(_float x, _float y, _float z);
	//void AddTriangleToTreeNode(_int hashKey, TRIANGLE triangle, _uint& index);
	//void CheckTriangles();
	//void GetLeafNodes(glm::vec3 vMain, glm::vec3 vTarget, std::vector<COctreeNode*>& vecNode);

//private:
//	void CheckDistanceOctree(_int index, glm::vec3 vCenter, _float fRadius, glm::vec3 vMain, glm::vec3 vDir, std::vector<COctreeNode*>& vecNode);

private:
	RESULT Ready(glm::vec3 vMax, glm::vec3 vMin, _uint depth);
	void ReadyOctree(_uint index, glm::vec3 vCenter, glm::vec3 vHalf, _uint depth, _uint maxDepth);
public:
	static COctree* Create(glm::vec3 vMax, glm::vec3 vMin, _uint depth);
};

NAMESPACE_END

#endif //_OCTREE_H_