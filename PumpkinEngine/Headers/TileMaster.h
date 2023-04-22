#ifndef _TILEMASTER_H_
#define _TILEMASTER_H_

#include "Base.h"
#include "glm\vec3.hpp"
#include "Tile.h"
#include <queue>

NAMESPACE_BEGIN(Engine)

// A class that generates and manages tiles
class ENGINE_API CTileMaster : public CBase
{
	SINGLETON(CTileMaster)
private:
	typedef struct sAStarNode
	{
		_float			fCost;
		_uint			iIndex;
		sAStarNode*		pParent;
	}NODE;

private:
	std::vector<CTile*>		m_vecTiles;

	_uint					m_iMapWidth;
	_uint					m_iMapHeight;

	std::list<NODE*>		m_openList;
	std::list<NODE*>		m_closeList;
	std::list<_uint>		m_bestList;

	_uint					m_iStartIdx;
	_uint					m_iEndIdx;

	_bool					m_bPathSetting;
	_bool					m_bMoving;

private:
	explicit CTileMaster();
	virtual ~CTileMaster();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready();
	// Add Tile Node to vector
	void AddTileNode(_uint idx, glm::vec3 vPos, _bool isBlocked, CGameObject* pObj);
	// Set Neighbor info
	void SetNeighbor(_uint tileIdx, _int eDir, _int targetIdx);
	// Notice player moving is end
	void SetMovingEnd();
	// Set new goal()
	void SetGoal(_uint idx);
	// Connect nodes
	void ConnectNeighbors();
	// Save Tile Data
	void SaveTileData(std::string fileName);
	void TileEnable(_int idx, bool value);
	void TileFill(_int idx, bool value);
public:
	_int FindNearestTile(glm::vec3 vPos);
	_int FindPickedTile(glm::vec3 vPos);
	glm::vec3 GetTilePos(_int idx);

private:
	_int GetNeighborIndex(glm::vec3 vPos);

public:
	// Start to find the closest path
	void PathfindingStart(_uint startIdx, _uint endIdx);
	// Clear all lists
	void ClearAllLists();
private:
	// Reset all tile information
	void ResetAllTiles();
	// Make A* Route
	void MakeRoute();
	// Check if the index is already in the list
	_bool ListCheck(_uint idx);
	// For diagonal neighbors, Check If neighbor nodes are traversable
	_bool DirCheck(_uint idx, CTile::TILE_DIR eDir);
	// Make new node
	NODE* MakeNode(_uint idx, NODE* pParent);
	// Sort function
	static _bool Compare(const NODE* pNode1, const NODE* pNode2);
	// Draw the best lists to gameobject
	void DrawPathNode();
	// Check if the tile is empty
	_bool EmptyCheck(CTile* pTile);

public:
	void GetBestList(std::queue<glm::vec3>& queue, _int count, _int& lastIdx);
};

NAMESPACE_END

#endif //_TILEMASTER_H_