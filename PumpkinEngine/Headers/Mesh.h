#ifndef _MESH_H_
#define _MESH_H_

#include "Component.h"
#include "EngineStruct.h"

NAMESPACE_BEGIN(Engine)

class CVIBuffer;
class CAABB;
class CTexture;
class CShader;
class CTransform;
class COpenGLDevice;

class ENGINE_API CMesh : public CComponent
{
private:
	COpenGLDevice*				m_pOpenGLDevice;
	CVIBuffer*					m_pVIBuffer;
	CAABB*						m_pBoundingBox_AABB;
	CTexture*					m_pDiffTexture;
	CShader*					m_pShader;
	CTransform*					m_pParentTransform;
	_bool						m_bWireFrame;
	_bool						m_bSelected;
	_bool						m_bDebug;

private:
	explicit CMesh();
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh();

public:
	virtual void Render();
private:
	virtual void Destroy();

public:
	CAABB* GetBoundingBoxAABB()						{ return m_pBoundingBox_AABB; }
	void SetTransform(CTransform* transform)		{ m_pParentTransform = transform; }
	void SetWireFrame(_bool wire)					{ m_bWireFrame = wire; }
	void SetSelcted(_bool select)					{ m_bSelected = select; }
	void SetDebugBox(_bool select)					{ m_bDebug = select; }

private:
	RESULT Ready(std::string ID, std::string filePath, std::string fileName, ModelType type, std::string shaderID, std::string texID_Diff);
	RESULT Ready_xyz_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_texUV_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_texUV_index_texNum(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	void Ready_Texture_Diff(std::string texID_Diff);
	void Ready_Shader(std::string shaderID);
public:
	virtual CComponent* Clone();
	static CMesh* Create(std::string ID, std::string filePath, std::string fileName, ModelType type, std::string shaderID, std::string texID_Diff);
};

NAMESPACE_END

#endif //_MESH_H_