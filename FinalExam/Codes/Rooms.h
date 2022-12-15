#ifndef _ROOMS_H_
#define _ROOMS_H_

#include "EngineDefines.h"
#include "Define.h"
#include "glm\vec3.hpp"

class Blocks;
class Rooms
{
public:
	_uint					m_iWidth;
	_uint					m_iHeight;
	_uint					m_iCornerIdx[4];
	glm::vec3				m_vCornerPos[4];
	std::vector<_uint>		m_vecEscape;

	std::vector<Blocks*>*	m_pVecBlocks;
	_uint					m_iTorchCount;
	
public:
	explicit Rooms();
	~Rooms();

public:
	void Ready(std::vector<Blocks*>* pVector, _uint roomIdx, _uint width, _uint height, _uint maxW, _uint maxH, _uint firstIdx);
};

#endif //_ROOMS_H_