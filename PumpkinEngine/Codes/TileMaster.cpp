#include "pch.h"

#include <fstream>
#include <sstream>

#include "..\Headers\TileMaster.h"
#include "..\Headers\OpenGLDefines.h"
#include "..\Headers\JsonParser.h"

USING(Engine)
USING(std)
USING(glm)
SINGLETON_FUNCTION(CTileMaster)

CTileMaster::CTileMaster()
	: m_iMapWidth(0), m_iMapHeight(0), m_iStartIdx(0), m_iEndIdx(0)
	, m_bPathSetting(false)
{
	m_vecTiles.clear();
}

CTileMaster::~CTileMaster()
{
}

// Call instead of destructor to manage class internal data
void CTileMaster::Destroy()
{
	list<NODE*>::iterator iter;
	for (iter = m_openList.begin(); iter != m_openList.end(); ++iter)
		delete* iter;
	m_openList.clear();

	for (iter = m_closeList.begin(); iter != m_closeList.end(); ++iter)
		delete* iter;
	m_closeList.clear();

	for (int i = 0; i < m_vecTiles.size(); ++i)
		SafeDestroy(m_vecTiles[i]);
	m_vecTiles.clear();

}

// Initialize
RESULT CTileMaster::Ready()
{
	ConnectNeighbors();

	return PK_NOERROR;
}

// Add Tile Node to vector
void CTileMaster::AddTileNode(_uint idx, vec3 vPos, _bool isBlocked, CGameObject* pObj)
{
	CTile* pTile = CTile::Create(idx, vPos, isBlocked, pObj);
	m_vecTiles.push_back(pTile);
}

void CTileMaster::SetNeighbor(_uint tileIdx, _int eDir, _int targetIdx)
{
	if (0 > tileIdx || tileIdx >= m_vecTiles.size())
		return;

	if (0 > targetIdx || targetIdx >= m_vecTiles.size())
		return;

	CTile* pTile = m_vecTiles[tileIdx];
	pTile->m_neighborNodes[eDir] = m_vecTiles[targetIdx];
}

void CTileMaster::SetMovingEnd()
{
	m_bMoving = false;
	m_bPathSetting = false;
	m_iStartIdx = m_iEndIdx;
	m_iEndIdx = INT_MAX;
	ClearAllLists();
	ResetAllTiles();
}

void CTileMaster::SetGoal(_uint idx)
{
	if (m_bMoving)
		return;

	if (idx >= m_vecTiles.size())
		return;
	
	if (m_vecTiles[idx]->GetBlocked())
		return;

	m_iEndIdx = idx;
	ClearAllLists();
	ResetAllTiles();

	PathfindingStart(m_iStartIdx, m_iEndIdx);
}

// Connect nodes
void CTileMaster::ConnectNeighbors()
{
	_int result = -1;
	vec3 vComparePos(0.f);

	printf("Start to connect neighbors of tiles...\n");
	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		CTile* pTile = m_vecTiles[i];

		// ZP
		vComparePos = pTile->m_vPos;
		vComparePos.z += 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZP] = m_vecTiles[result];

		// ZP_XP
		vComparePos = pTile->m_vPos;
		vComparePos.z += 5.f;
		vComparePos.x += 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZP_XP] = m_vecTiles[result];

		// XP
		vComparePos = pTile->m_vPos;
		vComparePos.x += 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::XP] = m_vecTiles[result];

		// ZN_XP
		vComparePos = pTile->m_vPos;
		vComparePos.z -= 5.f;
		vComparePos.x += 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZN_XP] = m_vecTiles[result];

		// ZN
		vComparePos = pTile->m_vPos;
		vComparePos.z -= 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZN] = m_vecTiles[result];

		// ZN_XN
		vComparePos = pTile->m_vPos;
		vComparePos.z -= 5.f;
		vComparePos.x -= 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZN_XN] = m_vecTiles[result];

		// XN
		vComparePos = pTile->m_vPos;
		vComparePos.x -= 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::XN] = m_vecTiles[result];

		// ZP_XN
		vComparePos = pTile->m_vPos;
		vComparePos.z += 5.f;
		vComparePos.x -= 5.f;
		result = GetNeighborIndex(vComparePos);
		if (-1 != result)
			pTile->m_neighborNodes[CTile::ZP_XN] = m_vecTiles[result];

		printf("%d, ", i);
		if (i % 10 == 9)
			printf("\n");
	}
	printf("Finished!\n");
}

void CTileMaster::SaveTileData(string fileName)
{
	vector< CJsonParser::sTileData> vecTiles;
	CJsonParser::sTileData tileData;

	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		CTile* pTile = m_vecTiles[i];
		tileData.ID = pTile->m_iIdx;
		tileData.POSITION = pTile->m_vPos;
		for (int j = 0; j < 8; ++j)
		{
			if (nullptr == pTile->m_neighborNodes[j])
				tileData.NEIGHBORS[j] = -1;
			else
				tileData.NEIGHBORS[j] = pTile->m_neighborNodes[j]->m_iIdx;
		}

		vecTiles.push_back(tileData);
	}

	CJsonParser::GetInstance()->SaveTileData(fileName, vecTiles);
}

void CTileMaster::TileEnable(_int idx, bool value)
{
	if (0 > idx || idx >= m_vecTiles.size())
		return;

	m_vecTiles[idx]->SetEnableTile(value);
}

void CTileMaster::TileFill(_int idx, bool value)
{
	if (0 > idx || idx >= m_vecTiles.size())
		return;

	m_vecTiles[idx]->m_bIsBlocked = value;
}

_int CTileMaster::FindNearestTile(vec3 vPos)
{
	_int idx = -1;
	_float fDist = FLT_MAX;
	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		_float dist = distance(m_vecTiles[i]->m_vPos, vPos);
		if (dist < fDist)
		{
			idx = m_vecTiles[i]->m_iIdx;
			fDist = dist;
		}
	}

	return idx;
}

_int CTileMaster::FindPickedTile(vec3 vPos)
{
	_int idx = -1;

	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		vec3 vTilePos = m_vecTiles[i]->m_vPos;
		if (vTilePos.x - 2.5f <= vPos.x &&
			vTilePos.x + 2.5f > vPos.x &&
			vTilePos.z - 2.5f <= vPos.z &&
			vTilePos.z + 2.5f > vPos.z)
		{
			idx = m_vecTiles[i]->m_iIdx;
			break;
		}
	}

	return idx;
}

vec3 CTileMaster::GetTilePos(_int idx)
{
	if (0 > idx || idx >= m_vecTiles.size())
		return vec3(0.f);

	return m_vecTiles[idx]->m_vPos;
}

_int CTileMaster::GetNeighborIndex(vec3 vPos)
{
	_int result = -1;
	_float fDist = 0.f;
	for (int i = 0; i < m_vecTiles.size(); ++i)
	{
		fDist = distance(m_vecTiles[i]->m_vPos, vPos);
		if (0.1f > fDist)
		{
			result = m_vecTiles[i]->m_iIdx;
			break;
		}
	}
	return result;
}

// Start to find the closest path
void CTileMaster::PathfindingStart(_uint startIdx, _uint endIdx)
{
	if (startIdx == endIdx)
		return;

	m_iStartIdx = startIdx;
	m_iEndIdx = endIdx;
	ClearAllLists();

	MakeRoute();
}

// Clear all lists
void CTileMaster::ClearAllLists()
{
	list<NODE*>::iterator iter;
	for (iter = m_openList.begin(); iter != m_openList.end(); ++iter)
		delete* iter;
	m_openList.clear();

	for (iter = m_closeList.begin(); iter != m_closeList.end(); ++iter)
		delete* iter;
	m_closeList.clear();

	list<_uint>::iterator iter2;
	for (iter2 = m_bestList.begin(); iter2 != m_bestList.end(); ++iter2)
		m_vecTiles[*iter2]->SetEnableTile(false);
	m_bestList.clear();
}

// Reset all tile information
void CTileMaster::ResetAllTiles()
{
	//for (int i = 0; i < m_vecTileObjs.size(); ++i)
	//{
	//	if (TILE_WALL != m_vecTileObjs[i]->GetTileType())
	//	{
	//		m_vecTiles[i]->m_eType = TILE_FLOOR;
	//		m_vecTileObjs[i]->ChangeTileType(TILE_FLOOR);
	//	}
	//}

	//if (m_iStartIdx < m_vecTileObjs.size())
	//{
	//	m_vecTiles[m_iStartIdx]->m_eType = TILE_START;
	//	m_vecTileObjs[m_iStartIdx]->ChangeTileType(TILE_START);
	//}
	//if (m_iEndIdx < m_vecTileObjs.size())
	//{
	//	m_vecTiles[m_iEndIdx]->m_eType = TILE_GOAL;
	//	m_vecTileObjs[m_iEndIdx]->ChangeTileType(TILE_GOAL);
	//}
}

// Make A* Route
void CTileMaster::MakeRoute()
{
	NODE* pFirstNode = new NODE;
	pFirstNode->iIndex = m_iStartIdx;
	pFirstNode->pParent = nullptr;
	pFirstNode->fCost = 0.f;
	m_closeList.push_back(pFirstNode);

	NODE* pCurNode = nullptr;
	while (true)
	{
		CTile* pNeighbor = nullptr;
		for (int i = 0; i < CTile::TILE_DIR_END; ++i)
		{
			pNeighbor = m_vecTiles[pFirstNode->iIndex]->m_neighborNodes[i];
			if (nullptr == pNeighbor)
				continue;

			if (DirCheck(pFirstNode->iIndex, (CTile::TILE_DIR)i) &&
				ListCheck(pNeighbor->m_iIdx))
			{
				pCurNode = MakeNode(pNeighbor->m_iIdx, pFirstNode);
				m_openList.push_back(pCurNode);
			}
		}

		if (m_openList.size() == 0)
			return;

		m_openList.sort(Compare);

		list<NODE*>::iterator iter = m_openList.begin();
		pFirstNode = (*iter);

		m_closeList.push_back(*iter);
		m_openList.erase(iter);

		if (pFirstNode->iIndex == m_iEndIdx)
		{
			while (true)
			{
				m_bestList.push_back(pFirstNode->iIndex);
				pFirstNode = pFirstNode->pParent;
				if (pFirstNode->iIndex == m_iStartIdx)
					break;
			}
			m_bestList.reverse();
			m_bPathSetting = true;
			DrawPathNode();
			break;
		}
	}
}

// Check if the index is already in the list
_bool CTileMaster::ListCheck(_uint idx)
{
	list<NODE*>::iterator iter;
	for (iter = m_openList.begin(); iter != m_openList.end(); ++iter)
	{
		if ((*iter)->iIndex == idx)
			return false;
	}

	for (iter = m_closeList.begin(); iter != m_closeList.end(); ++iter)
	{
		if ((*iter)->iIndex == idx)
			return false;
	}

	return true;
}

// For diagonal neighbors, Check If neighbor nodes are traversable
_bool CTileMaster::DirCheck(_uint idx, CTile::TILE_DIR eDir)
{
	CTile* pTile = m_vecTiles[idx];

	switch (eDir)
	{
	case CTile::ZP:
	case CTile::XP:
	case CTile::ZN:
	case CTile::XN:
		return true;

	case CTile::ZP_XP:
	{
		if (nullptr == pTile->m_neighborNodes[CTile::ZP] ||
			//!EmptyCheck(pTile->m_neighborNodes[CTile::ZP]) ||
			nullptr == pTile->m_neighborNodes[CTile::XP])
			//!EmptyCheck(pTile->m_neighborNodes[CTile::XP]))
			return false;
		else
			return true;
	}

	case CTile::ZN_XP:
	{
		if (nullptr == pTile->m_neighborNodes[CTile::ZN] ||
			//!EmptyCheck(pTile->m_neighborNodes[CTile::ZN]) ||
			nullptr == pTile->m_neighborNodes[CTile::XP])
			//!EmptyCheck(pTile->m_neighborNodes[CTile::XP]))
			return false;
		else
			return true;
	}

	case CTile::ZN_XN:
	{
		if (nullptr == pTile->m_neighborNodes[CTile::ZN] ||
			//!EmptyCheck(pTile->m_neighborNodes[CTile::ZN]) ||
			nullptr == pTile->m_neighborNodes[CTile::XN])
			//!EmptyCheck(pTile->m_neighborNodes[CTile::XN]))
			return false;
		else
			return true;
	}

	case CTile::ZP_XN:
	{
		if (nullptr == pTile->m_neighborNodes[CTile::ZP] ||
			//!EmptyCheck(pTile->m_neighborNodes[CTile::ZP]) ||
			nullptr == pTile->m_neighborNodes[CTile::XN])
			//!EmptyCheck(pTile->m_neighborNodes[CTile::XN]))
			return false;
		else
			return true;
	}
		
	}

	return false;
}

// Make new node
CTileMaster::NODE* CTileMaster::MakeNode(_uint idx, NODE* pParent)
{
	NODE* pNode = new NODE;
	pNode->iIndex = idx;
	pNode->pParent = pParent;

	vec3 vPos = m_vecTiles[idx]->m_vPos;
	vec3 vParentPos = m_vecTiles[pParent->iIndex]->m_vPos;
	vec3 vEndPos = m_vecTiles[m_iEndIdx]->m_vPos;

	_float fDist1 = distance(vPos, vParentPos);
	_float fDist2 = distance(vPos, vEndPos);

	pNode->fCost = fDist1 + fDist2;

	return pNode;
}

// Sort function
_bool CTileMaster::Compare(const NODE* pNode1, const NODE* pNode2)
{
	return pNode1->fCost < pNode2->fCost;
}

// Draw the best lists to gameobject
void CTileMaster::DrawPathNode()
{
	list<_uint>::iterator iter;
	for (iter = m_bestList.begin(); iter != m_bestList.end(); ++iter)
	{
		m_vecTiles[*iter]->SetEnableTile(true);
	}
}

_bool CTileMaster::EmptyCheck(CTile* pTile)
{
	if (pTile->GetBlocked())
		return false;

	return true;
}

void CTileMaster::GetBestList(std::queue<glm::vec3>& queue, _int count, _int& lastIdx)
{
	_int curCount = 0;
	vector<CTile*> vecBest;
	list<_uint>::iterator iter;
	for (iter = m_bestList.begin(); iter != m_bestList.end(); ++iter)
	{
		if (curCount >= count)
			break;

		++curCount;

		vecBest.push_back(m_vecTiles[(*iter)]);

		//CTile* tile = m_vecTiles[(*iter)];
		//queue.push(tile->m_vPos);
		//lastIdx = tile->m_iIdx;
	}

	_int cutCount = vecBest.size();
	for (int i = vecBest.size() - 1; i >= 0; --i)
	{
		if (vecBest[i]->m_bIsBlocked)
			--cutCount;
		else
			break;
	}

	for (int i = 0; i < cutCount; ++i)
	{
		queue.push(vecBest[i]->m_vPos);
		lastIdx = vecBest[i]->m_iIdx;
	}

	ClearAllLists();
}

