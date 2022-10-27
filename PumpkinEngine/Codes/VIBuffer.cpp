#include "pch.h"
#include "..\Headers\VIBuffer.h"
#include "..\Headers\OpenGLDefines.h"

USING(Engine)

CVIBuffer::CVIBuffer()
	: m_strName(""), m_iVAO_ID(0), m_iVB_ID(0), m_iNumVtx(0), m_iIB_ID(0), m_iNumIdx(0)
	, m_bWireFrame(false)
{
}

CVIBuffer::~CVIBuffer()
{
}

void CVIBuffer::Render()
{
	glBindVertexArray(m_iVAO_ID);
	if (m_bWireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, m_iNumIdx, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CVIBuffer::Destroy()
{
	glDeleteVertexArrays(1, &m_iVAO_ID);
	glDeleteBuffers(1, &m_iVB_ID);
	glDeleteBuffers(1, &m_iIB_ID);
}

RESULT CVIBuffer::Ready(_uint numVTX, VTX* pVertices, _uint numIDX, _uint* pIndices, ModelType type)
{
	if (nullptr == pVertices || nullptr == pIndices)
		return PK_ERROR_NULLPTR;

	m_iNumVtx = numVTX;
	m_iNumIdx = numIDX;

	glGenVertexArrays(1, &m_iVAO_ID);
	glBindVertexArray(m_iVAO_ID);

	glGenBuffers(1, &m_iVB_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_iVB_ID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VTX) * m_iNumVtx, pVertices, GL_STATIC_DRAW);

	switch (type)
	{
	case xyz_index:
		Ready_xyz();
		break;
	case xyz_normal_index:
		Ready_xyz_normal();
		break;

	case xyz_normal_texUV_index:
	case xyz_normal_texUV_index_texNum:
		Ready_xyz_normal_texUV();
		break;
	}

	glGenBuffers(1, &m_iIB_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iIB_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_uint) * m_iNumIdx, pIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return PK_NOERROR;
}

void CVIBuffer::Ready_xyz()
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VTX), 0);
}

void CVIBuffer::Ready_xyz_normal()
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VTX), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VTX), (void*)offsetof(VTX, vNormal.x));
}

void CVIBuffer::Ready_xyz_normal_texUV()
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VTX), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VTX), (void*)offsetof(VTX, vNormal.x));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VTX), (void*)offsetof(VTX, vTexUV.x));
}

CVIBuffer* CVIBuffer::Create(_uint numVTX, VTX* pVertices, _uint numIDX, _uint* pIndices, ModelType type)
{
	CVIBuffer* pInstance = new CVIBuffer();
	if (PK_NOERROR != pInstance->Ready(numVTX, pVertices, numIDX, pIndices, type))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
