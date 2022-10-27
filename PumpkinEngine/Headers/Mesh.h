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
	CTransform*					m_pTransform;

private:
	explicit CMesh();
	explicit CMesh(const CMesh& rhs);
	virtual ~CMesh();

public:
	virtual void Render();
private:
	virtual void Destroy();

public:
	CAABB* GetBoundingBoxAABB() { return m_pBoundingBox_AABB; }
	void SetTransform(CTransform* transform) { m_pTransform = transform; }

private:
	RESULT Ready(std::string filePath, std::string fileName, ModelType type, std::string shaderID, std::string vertexPath, std::string fragmentPath);
	RESULT Ready_xyz_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_texUV_index(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	RESULT Ready_xyz_normal_texUV_index_texNum(std::string filePath, std::string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum);
	void Ready_Texture(std::string path, std::string texName);
	void Ready_Shader(std::string shaderID, std::string vertexPath, std::string fragmentPath);
public:
	virtual CComponent* Clone();
	static CMesh* Create(std::string filePath, std::string fileName, ModelType type, std::string shaderID, std::string vertexPath, std::string fragmentPath);
};

NAMESPACE_END

#endif //_MESH_H_