#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "EngineDefines.h"
#include "Define.h"
#include "glm\vec3.hpp"

class Blocks
{
public:
	_uint		m_iIndex;
	glm::vec3	m_vCenterPos;
	_bool		m_bIsExist;

	_bool		m_bIsWall[4];//up, left, right, down
	_bool		m_bIsDoor[4];
	_bool		m_bIsTorch[4];

	_bool		m_bPath;
	_bool		m_bTorch;
	_uint		m_iRoomIndex;

public:
	explicit Blocks();
	~Blocks();

public:
	void Ready(_uint idx, glm::vec3 vCenterPos);
};

#endif //_BLOCKS_H_