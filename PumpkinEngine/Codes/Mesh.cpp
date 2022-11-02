#include "pch.h"
#include "../Headers/Mesh.h"
#include "../Headers/VIBuffer.h"
#include "../Headers/Component.h"
#include "../Headers/ComponentMaster.h"
#include "../Headers/OpenGLDevice.h"
#include "../Headers/Texture.h"
#include "../Headers/Shader.h"
#include "../Headers/Transform.h"
#include "../Headers/AABB.h"
#include <fstream>
#include <sstream>

USING(Engine)
USING(std)
USING(glm)

CMesh::CMesh()
    : m_pOpenGLDevice(COpenGLDevice::GetInstance())
    , m_pVIBuffer(nullptr)
    , m_pBoundingBox_AABB(nullptr)
    , m_pDiffTexture(nullptr)
    , m_pShader(nullptr)
    , m_pParentTransform(nullptr)
    , m_bWireFrame(false)
    , m_bSelected(false)
    , m_bDebug(false)
{
    m_pOpenGLDevice->AddRefCnt();
}

CMesh::CMesh(const CMesh& rhs)
    : m_pOpenGLDevice(rhs.m_pOpenGLDevice)
    , m_pVIBuffer(rhs.m_pVIBuffer)
    , m_pDiffTexture(rhs.m_pDiffTexture)
    , m_pShader(rhs.m_pShader)
    , m_pParentTransform(nullptr)
    , m_bWireFrame(rhs.m_bWireFrame)
    , m_bSelected(rhs.m_bSelected)
    , m_bDebug(rhs.m_bDebug)
{
    m_tag = rhs.m_tag;
    m_pOpenGLDevice->AddRefCnt();
    if (nullptr != m_pVIBuffer) m_pVIBuffer->AddRefCnt();
    if (nullptr != m_pDiffTexture) m_pDiffTexture->AddRefCnt();
    if (nullptr != m_pShader) m_pShader->AddRefCnt();

    m_pBoundingBox_AABB = CAABB::Create(rhs.m_pBoundingBox_AABB->m_vCenter
        , rhs.m_pBoundingBox_AABB->m_vHalfExtents
        , rhs.m_pBoundingBox_AABB->m_vMin
        , rhs.m_pBoundingBox_AABB->m_vMax
        , rhs.m_pBoundingBox_AABB->m_shaderID);
}

CMesh::~CMesh()
{
}

void CMesh::Render()
{
    if (nullptr == m_pParentTransform || nullptr == m_pShader)
        return;
    
    mat4x4 matWorld = *m_pParentTransform->GetWorldMatrix();
    const mat4x4 matView = m_pOpenGLDevice->GetViewMatrix();
    const mat4x4 matProj = m_pOpenGLDevice->GetProjMatrix();
    m_pShader->SetMatrixInfo(matWorld, matView, matProj);
    m_pShader->SetLightEnableInfo(!m_bWireFrame);
    m_pShader->SetSelected(m_bSelected);

    if (nullptr != m_pDiffTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pDiffTexture->GetTextureID());
    }

	if (nullptr != m_pVIBuffer)
    {
        m_pVIBuffer->SetWireFrame(m_bWireFrame);
        m_pVIBuffer->Render();
    }

    if (m_bDebug && nullptr != m_pBoundingBox_AABB)
        m_pBoundingBox_AABB->Render();
}

void CMesh::Destroy()
{
    SafeDestroy(m_pOpenGLDevice);
    SafeDestroy(m_pVIBuffer);
    SafeDestroy(m_pBoundingBox_AABB);
    SafeDestroy(m_pDiffTexture);
    SafeDestroy(m_pShader);
    m_pParentTransform = nullptr;

	CComponent::Destroy();
}

RESULT CMesh::Ready(string ID, string filePath, string fileName, ModelType type, string shaderID, string texID_Diff)
{
    m_tag = ID;

    VTX* pVertices = nullptr;
    _uint* pIndices = nullptr;
    _uint vertexNum = 0;
    _uint indexNum = 0;

    switch (type)
    {
    case xyz_index:
        Ready_xyz_index(filePath, fileName, &pVertices, &pIndices, vertexNum, indexNum);
        break;

    case xyz_normal_index:
        Ready_xyz_normal_index(filePath, fileName, &pVertices, &pIndices, vertexNum, indexNum);
        break;

    case xyz_normal_texUV_index:
        Ready_xyz_normal_texUV_index(filePath, fileName, &pVertices, &pIndices, vertexNum, indexNum);
        break;

    case xyz_normal_texUV_index_texNum:
        Ready_xyz_normal_texUV_index_texNum(filePath, fileName, &pVertices, &pIndices, vertexNum, indexNum);
        break;
    }
	
    m_pVIBuffer = CVIBuffer::Create(vertexNum, pVertices, indexNum, pIndices, type);
    
    delete[] pVertices;
    delete[] pIndices;

    Ready_Texture_Diff(texID_Diff);
    Ready_Shader(shaderID);

    if (nullptr != m_pShader)
        m_pShader->SetTextureInfo();

	return PK_NOERROR;
}

RESULT CMesh::Ready_xyz_index(string filePath, string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum)
{
    //mesh ¸¸µé°í 

    return RESULT();
}

RESULT CMesh::Ready_xyz_normal_index(string filePath, string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum)
{
    stringstream ss;
    ss << filePath << fileName;
    ifstream file(ss.str());
    if (!file.is_open())
        return PK_ERROR_MESHFILE_OPEN;

    _uint triangleNum = 0;
    const _uint bufSize = 10000;
    char buffer[bufSize];
    file.getline(buffer, bufSize);

    string next;
    while (file >> next)
    {
        if (next == "end_header")
            break;

        if (next == "vertex")
            file >> vertexNum;

        if (next == "face")
            file >> triangleNum;
    }

    vec3 vMin = vec3(FLT_MAX);
    vec3 vMax = vec3(FLT_MIN);

    *pVertices = new VTX[vertexNum];
    memset(*pVertices, 0, sizeof(**pVertices));
    for (_uint i = 0; i < vertexNum; ++i)
    {
        vec4& vPos = (*pVertices)[i].vPos;
        file >> vPos.x;
        file >> vPos.y;
        file >> vPos.z;
        vPos.w = 1.f;
        file >> (*pVertices)[i].vNormal.x;
        file >> (*pVertices)[i].vNormal.y;
        file >> (*pVertices)[i].vNormal.z;
        (*pVertices)[i].vNormal.w = 1.f;

        if (vMin.x > vPos.x)
            vMin.x = vPos.x;
        if (vMin.y > vPos.y)
            vMin.y = vPos.y;
        if (vMin.z > vPos.z)
            vMin.z = vPos.z;
        if (vMax.x < vPos.x)
            vMax.x = vPos.x;
        if (vMax.y < vPos.y)
            vMax.y = vPos.y;
        if (vMax.z < vPos.z)
            vMax.z = vPos.z;
    }
    vec3 vHalf = (vMax - vMin) / 2.f;
    vec3 vCenter = vMin + vHalf;
    m_pBoundingBox_AABB = CAABB::Create(vCenter, vHalf, vMin, vMax, "DebugBoxShader");

    IDX* pTriangles = new IDX[triangleNum];
    memset(pTriangles, 0, sizeof(*pTriangles));
    int discard = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        file >> discard;
        file >> pTriangles[i]._0;
        file >> pTriangles[i]._1;
        file >> pTriangles[i]._2;
    }
    file.close();

    indexNum = triangleNum * 3;
    *pIndices = new _uint[indexNum];
    memset(*pIndices, 0, sizeof(**pIndices));
    _uint indexCount = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        (*pIndices)[indexCount + 0] = pTriangles[i]._0;
        (*pIndices)[indexCount + 1] = pTriangles[i]._1;
        (*pIndices)[indexCount + 2] = pTriangles[i]._2;

        indexCount += 3;
    }

    delete[] pTriangles;

    return PK_NOERROR;
}

RESULT CMesh::Ready_xyz_normal_texUV_index(std::string filePath, string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum) 
{
    stringstream ss;
    ss << filePath << fileName;
    ifstream file(ss.str());
    if (!file.is_open())
        return PK_ERROR_MESHFILE_OPEN;

    _uint triangleNum = 0;
    const _uint bufSize = 10000;
    char buffer[bufSize];
    file.getline(buffer, bufSize);

    string next;
    while (file >> next)
    {
        if (next == "end_header")
            break;

        if (next == "vertex")
            file >> vertexNum;

        if (next == "face")
            file >> triangleNum;
    }

    vec3 vMin = vec3(FLT_MAX);
    vec3 vMax = vec3(FLT_MIN);

    *pVertices = new VTX[vertexNum];
    memset(*pVertices, 0, sizeof(**pVertices));
    for (_uint i = 0; i < vertexNum; ++i)
    {
        vec4& vPos = (*pVertices)[i].vPos;
        file >> vPos.x;
        file >> vPos.y;
        file >> vPos.z;
        vPos.w = 1.f;
        file >> (*pVertices)[i].vNormal.x;
        file >> (*pVertices)[i].vNormal.y;
        file >> (*pVertices)[i].vNormal.z;
        (*pVertices)[i].vNormal.w = 1.f;
        file >> (*pVertices)[i].vTexUV.x;
        file >> (*pVertices)[i].vTexUV.y;

        if (vMin.x > vPos.x)
            vMin.x = vPos.x;
        if (vMin.y > vPos.y)
            vMin.y = vPos.y;
        if (vMin.z > vPos.z)
            vMin.z = vPos.z;
        if (vMax.x < vPos.x)
            vMax.x = vPos.x;
        if (vMax.y < vPos.y)
            vMax.y = vPos.y;
        if (vMax.z < vPos.z)
            vMax.z = vPos.z;
    }
    vec3 vHalf = (vMax - vMin) / 2.f;
    vec3 vCenter = vMin + vHalf;
    m_pBoundingBox_AABB = CAABB::Create(vCenter, vHalf, vMin, vMax, "DebugBoxShader");

    IDX* pTriangles = new IDX[triangleNum];
    memset(pTriangles, 0, sizeof(*pTriangles));
    int discard = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        file >> discard;
        file >> pTriangles[i]._0;
        file >> pTriangles[i]._1;
        file >> pTriangles[i]._2;
    }
    file.close();

    indexNum = triangleNum * 3;
    *pIndices = new _uint[indexNum];
    memset(*pIndices, 0, sizeof(**pIndices));
    _uint indexCount = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        (*pIndices)[indexCount + 0] = pTriangles[i]._0;
        (*pIndices)[indexCount + 1] = pTriangles[i]._1;
        (*pIndices)[indexCount + 2] = pTriangles[i]._2;

        indexCount += 3;
    }

    delete[] pTriangles;

    return PK_NOERROR;
}

RESULT CMesh::Ready_xyz_normal_texUV_index_texNum(std::string filePath, string fileName, VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum)
{
    stringstream ss;
    ss << filePath << fileName;
    ifstream file(ss.str());
    if (!file.is_open())
        return PK_ERROR_MESHFILE_OPEN;

    _uint triangleNum = 0;
    const _uint bufSize = 10000;
    char buffer[bufSize];
    file.getline(buffer, bufSize);

    string next;
    while (file >> next)
    {
        if (next == "end_header")
            break;

        if (next == "vertex")
            file >> vertexNum;

        if (next == "face")
            file >> triangleNum;
    }

    vec3 vMin = vec3(FLT_MAX);
    vec3 vMax = vec3(FLT_MIN);

    *pVertices = new VTX[vertexNum];
    memset(*pVertices, 0, sizeof(**pVertices));
    for (_uint i = 0; i < vertexNum; ++i)
    {
        vec4& vPos = (*pVertices)[i].vPos;
        file >> vPos.x;
        file >> vPos.y;
        file >> vPos.z;
        vPos.w = 1.f;
        file >> (*pVertices)[i].vNormal.x;
        file >> (*pVertices)[i].vNormal.y;
        file >> (*pVertices)[i].vNormal.z;
        (*pVertices)[i].vNormal.w = 1.f;
        file >> (*pVertices)[i].vTexUV.x;
        file >> (*pVertices)[i].vTexUV.y;

        if (vMin.x > vPos.x)
            vMin.x = vPos.x;
        if (vMin.y > vPos.y)
            vMin.y = vPos.y;
        if (vMin.z > vPos.z)
            vMin.z = vPos.z;
        if (vMax.x < vPos.x)
            vMax.x = vPos.x;
        if (vMax.y < vPos.y)
            vMax.y = vPos.y;
        if (vMax.z < vPos.z)
            vMax.z = vPos.z;
    }
    vec3 vHalf = (vMax - vMin) / 2.f;
    vec3 vCenter = vMin + vHalf;
    m_pBoundingBox_AABB = CAABB::Create(vCenter, vHalf, vMin, vMax, "DebugBoxShader");

    IDX* pTriangles = new IDX[triangleNum];
    memset(pTriangles, 0, sizeof(*pTriangles));
    int discard = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        file >> discard;
        file >> pTriangles[i]._0;
        file >> pTriangles[i]._1;
        file >> pTriangles[i]._2;
        file >> discard;
    }
    file.close();

    indexNum = triangleNum * 3;
    *pIndices = new _uint[indexNum];
    memset(*pIndices, 0, sizeof(**pIndices));
    _uint indexCount = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        (*pIndices)[indexCount + 0] = pTriangles[i]._0;
        (*pIndices)[indexCount + 1] = pTriangles[i]._1;
        (*pIndices)[indexCount + 2] = pTriangles[i]._2;

        indexCount += 3;
    }

    delete[] pTriangles;

    return PK_NOERROR;
}

void CMesh::Ready_Texture_Diff(string texID)
{
    CComponent* pComponent = CloneComponent<CTexture*>(texID);
    if (nullptr != pComponent)
        m_pDiffTexture = dynamic_cast<CTexture*>(pComponent);
}

void CMesh::Ready_Shader(string shaderID)
{
    CComponent* pComponent = CloneComponent<CShader*>(shaderID);
    if (nullptr != pComponent)
        m_pShader = dynamic_cast<CShader*>(pComponent);
}

CComponent* CMesh::Clone()
{
    return new CMesh(*this);
}

CMesh* CMesh::Create(string ID, string filePath, string fileName, ModelType type, string shaderID, string texID_Diff)
{
	CMesh* pInstance = new CMesh();
	if (PK_NOERROR != pInstance->Ready(ID, filePath, fileName, type, shaderID, texID_Diff))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

