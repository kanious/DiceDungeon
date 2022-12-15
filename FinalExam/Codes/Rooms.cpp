#include "Rooms.h"
#include "Blocks.h"

USING(glm)
USING(std)

Rooms::Rooms()
	: m_iWidth(0), m_iHeight(0), m_pVecBlocks(nullptr), m_iTorchCount(0)
{
	memset(m_iCornerIdx, 0, sizeof(m_iCornerIdx));
	memset(m_vCornerPos, 0, sizeof(m_vCornerPos));
	m_vecEscape.clear();
}

Rooms::~Rooms()
{
}

void Rooms::Ready(vector<Blocks*>* pVector, _uint roomIdx, _uint width, _uint height, _uint maxW, _uint maxH, _uint firstIdx)
{
	m_pVecBlocks = pVector;
	m_iWidth = width;
	m_iHeight = height;
	
	m_iCornerIdx[0] = firstIdx;
	m_iCornerIdx[1] = firstIdx + width - 1;
	m_iCornerIdx[2] = firstIdx + ((height - 1) * maxW);
	m_iCornerIdx[3] = m_iCornerIdx[2] + width - 1;

	Blocks* block;
	for (_uint i = 0; i < height; ++i)
	{
		for (_uint j = 0; j < width; ++j)
		{
			_uint curIdx = firstIdx + (i * maxW) + j;
			block = (*m_pVecBlocks)[curIdx];
			block->m_bIsExist = true;
			block->m_iRoomIndex = roomIdx;

			// up
			if (0 == i)
			{
				if (curIdx < maxW)
					block->m_bIsWall[0] = true;
				else
				{
					if ((*m_pVecBlocks)[curIdx - maxW]->m_bIsExist)
						block->m_bIsDoor[0] = true;
					else
						block->m_bIsWall[0] = true;
				}
			}

			// left
			if (0 == j)
			{
				if (0 == curIdx % maxW)
					block->m_bIsWall[1] = true;
				else
				{
					if ((*m_pVecBlocks)[curIdx - 1]->m_bIsExist)
						block->m_bIsDoor[1] = true;
					else
						block->m_bIsWall[1] = true;
				}
			}

			// right
			if (width - 1 == j)
			{
				if (maxW - 1 == curIdx % maxW)
					block->m_bIsWall[2] = true;
				else
				{
					if ((*m_pVecBlocks)[curIdx + 1]->m_bIsExist)
						block->m_bIsDoor[2] = true;
					else
						block->m_bIsWall[2] = true;
				}
			}

			// down
			if (height - 1 == i)
			{
				if ((maxW * maxH) - maxW <= curIdx)
					block->m_bIsWall[3] = true;
				else
				{
					if ((*m_pVecBlocks)[curIdx + maxW]->m_bIsExist)
						block->m_bIsDoor[3] = true;
					else
						block->m_bIsWall[3] = true;
				}
			}
		}
	}





	//_bool isWall[4];
	//_bool isDoor[4];
	//_bool isTorch[4];

	//newBlock->SetDetailInifo(true, isWall, isDoor, isTorch);
}
