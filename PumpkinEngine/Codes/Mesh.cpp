#include "pch.h"
#include "../Headers/Mesh.h"
#include "../Headers/VIBuffer.h"
#include "../Headers/Component.h"
#include "../Headers/ComponentMaster.h"
#include "../Headers/OpenGLDevice.h"
#include "../Headers/Texture.h"
#include "../Headers/Shader.h"
#include "../Headers/Transform.h"
#include "../Headers/BoundingBox.h"
#include "../Headers/AnimController.h"
#include "../Headers/AnimationData.h"
#include "../Headers/Animation.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>


USING(Engine)
USING(std)
USING(glm)

static int GetRandNum(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}

CMesh::CMesh()
    : m_pOpenGLDevice(COpenGLDevice::GetInstance())
    , m_pBoundingBox(nullptr)
    , m_pDiffTexture(nullptr)
    , m_pNormalTexture(nullptr)
    , m_pShader(nullptr)
    , m_pParentTransform(nullptr)
    , m_bWireFrame(false)
    , m_bSelected(false)
    , m_bDebug(false)
    , m_bTransparency(false)
    , m_bBiilboard(false)
    , m_bPriority(false)
    , m_textureFileName("")
    , m_iTriNum(0)
    , m_pTriangles(nullptr)
    , m_pAnimController(nullptr)
    , m_initSize("")
    , m_meshType("")
    , m_pMatrix(nullptr)
    , m_bHasBone(false)
{
    m_pOpenGLDevice->AddRefCnt();
    m_vecVIBuffers.clear();
    m_mapBoneInformation.clear();
}

CMesh::CMesh(const CMesh& rhs)
    : m_pOpenGLDevice(rhs.m_pOpenGLDevice)
    , m_pDiffTexture(rhs.m_pDiffTexture)
    , m_pNormalTexture(rhs.m_pNormalTexture)
    , m_pShader(rhs.m_pShader)
    , m_pParentTransform(nullptr)
    , m_bWireFrame(rhs.m_bWireFrame)
    , m_bSelected(rhs.m_bSelected)
    , m_bDebug(rhs.m_bDebug)
    , m_bBiilboard(rhs.m_bBiilboard)
    , m_bPriority(rhs.m_bPriority)
    , m_bTransparency(rhs.m_bTransparency)
    , m_textureFileName(rhs.m_textureFileName)
    , m_iTriNum(rhs.m_iTriNum)
    , m_pAnimController(nullptr)
    , m_initSize(rhs.m_initSize)
    , m_meshType(rhs.m_meshType)
    , m_pMatrix(rhs.m_pMatrix)
    , m_bHasBone(rhs.m_bHasBone)
    , m_mapBoneInformation(rhs.m_mapBoneInformation)
{
    m_tag = rhs.m_tag;
    m_pOpenGLDevice->AddRefCnt();
    for (int i = 0; i < rhs.m_vecVIBuffers.size(); ++i)
    {
        m_vecVIBuffers.push_back(rhs.m_vecVIBuffers[i]);
        m_vecVIBuffers[i]->AddRefCnt();
    }
    //if (nullptr != m_pVIBuffer) m_pVIBuffer->AddRefCnt();
    //if (nullptr != m_pDiffTexture) m_pDiffTexture->AddRefCnt();
    //if (nullptr != m_pNormalTexture) m_pNormalTexture->AddRefCnt();
    if (nullptr != m_pShader) m_pShader->AddRefCnt();

    if (nullptr != rhs.m_pBoundingBox)
    {
        m_pBoundingBox = CBoundingBox::Create(
            rhs.m_pBoundingBox->m_vMin
            , rhs.m_pBoundingBox->m_vMax
            , rhs.m_pBoundingBox->m_shaderID);
    }

    //m_pTriangles = new TRIANGLE[rhs.m_iTriNum];
    //memcpy(m_pTriangles, rhs.m_pTriangles, sizeof(TRIANGLE) * rhs.m_iTriNum);
;}

CMesh::~CMesh()
{
}

// Basic Render Function, render 3D mesh with shader/texture information
void CMesh::Render()
{
    if (nullptr == m_pParentTransform || nullptr == m_pShader)
        return;
    
    mat4x4 matWorld = mat4x4(1.f);
    if (nullptr != m_pMatrix)
        matWorld = *m_pMatrix * matWorld;
    matWorld = *m_pParentTransform->GetWorldMatrix() * matWorld;
    const mat4x4 matView = m_pOpenGLDevice->GetViewMatrix();
    const mat4x4 matProj = m_pOpenGLDevice->GetProjMatrix();

    if (!m_bBiilboard)
    {
        //if (nullptr != m_pAnimController)
        //{
        //    mat4x4 matAnim = m_pAnimController->GetMatrix();
        //    vec3 vScale = m_pParentTransform->GetScale();
        //    vScale.x = 1.f / vScale.x;
        //    vScale.y = 1.f / vScale.y;
        //    vScale.z = 1.f / vScale.z;
        //    matAnim[3][0] *= vScale.x;
        //    matAnim[3][1] *= vScale.y;
        //    matAnim[3][2] *= vScale.z;
        //    matWorld = matWorld * matAnim;
        //}

        m_pShader->SetMatrixInfo(matWorld, matView, matProj);
    }
    else
    {
        mat4x4 newView = mat4x4(matView);
        newView[3] = vec4(0.f);
        mat4x4 matViewInv = inverse(matView);
        matViewInv[3] = matWorld[3];
        vec3 vScale = m_pParentTransform->GetScale();
        for (int i = 0; i < 3; ++i)
        {
            matViewInv[0][i] *= vScale.x;
            matViewInv[1][i] *= vScale.y;
            matViewInv[2][i] *= vScale.z;
        }
        m_pShader->SetMatrixInfo(matViewInv, matView, matProj);
        m_pShader->SetMatrixInfo(matWorld, matView, matProj);
    }

    m_pShader->SetLightEnableInfo(!m_bWireFrame);
    m_pShader->SetSelected(m_bSelected);
    m_pShader->SetTransparency(m_bTransparency);

    // Animation
    if (m_bHasBone)
    {
        vector<mat4x4>* pVec = m_pAnimController->GetMatrix();
        for (int i = 0; i < pVec->size(); ++i)
            m_pShader->SetBoneMatrices(i, (*pVec)[i]);
    }

    // Animation Blending
    if (nullptr != m_pAnimController)
    {
        if (m_pAnimController->GetBlending())
        {
            vector<mat4x4>* pVec = m_pAnimController->GetPrevMatrix();
            for (int i = 0; i < pVec->size(); ++i)
                m_pShader->SetPrevBoneMatrices(i, (*pVec)[i]);
            m_pShader->SetIsBlending(true);
            m_pShader->SetBlendingFactor(m_pAnimController->GetBlendingFactor());
            m_pAnimController->BlendingMove();

            if (0.95f < m_pAnimController->GetBlendingFactor())
            {
                m_pAnimController->InitBlendingVariables();
            }
        }
        else
        {
            m_pShader->SetIsBlending(false);
            m_pShader->SetBlendingFactor(1.f);
        }
    }


    if (nullptr != m_pDiffTexture)
    {
        m_pShader->SetTextureInfo();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pDiffTexture->GetTextureID());
    }

    if (nullptr != m_pNormalTexture)
    {
        m_pShader->SetNormalTextureInfo();
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_pNormalTexture->GetTextureID());
    }

	//if (nullptr != m_pVIBuffer)
    if (0 < m_vecVIBuffers.size())
    {
        if (m_bTransparency)
            glDepthMask(GL_FALSE);
        else
            glDepthMask(GL_TRUE);

        if (m_bPriority)
            glDisable(GL_DEPTH_TEST);

        for (int i = 0; i < m_vecVIBuffers.size(); ++i)
        {
            m_vecVIBuffers[i]->SetWireFrame(m_bWireFrame);
            m_vecVIBuffers[i]->Render();
        }

        // Outline Effect
   //     if (!m_bSelected)
   //     {
   //         m_pVIBuffer->Render();
   //     }
   //     else
   //     {
   //         glEnable(GL_STENCIL_TEST);

			//glStencilFunc(GL_ALWAYS, 1, 0xFF);
			//glStencilMask(0xFF);
			//m_pShader->SetSelected(false);
			//m_pVIBuffer->Render();

			//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			//glStencilMask(0x00);
			//glDisable(GL_DEPTH_TEST);
			//m_pShader->SetSelected(true);
			//matWorld = scale(matWorld, vec3(1.02f));
			//m_pShader->SetMatrixInfo(matWorld, matView, matProj);
			//m_pVIBuffer->Render();

			//glStencilMask(0xFF);
			//glStencilFunc(GL_ALWAYS, 0, 0xFF);
			//glEnable(GL_DEPTH_TEST);
   //         glDisable(GL_STENCIL_TEST);
   //     }

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

    if (m_bDebug && nullptr != m_pBoundingBox)
        m_pBoundingBox->Render();
}

// Call instead of destructor to manage class internal data
void CMesh::Destroy()
{
    SafeDestroy(m_pOpenGLDevice);
    for (int i = 0; i < m_vecVIBuffers.size(); ++i)
        SafeDestroy(m_vecVIBuffers[i]);
    SafeDestroy(m_pBoundingBox);
    //SafeDestroy(m_pDiffTexture);
    //SafeDestroy(m_pNormalTexture);
    SafeDestroy(m_pShader);
    m_pParentTransform = nullptr;
    if (nullptr != m_pTriangles)
        delete m_pTriangles;
    if (nullptr != m_pMatrix)
        delete m_pMatrix;

	CComponent::Destroy();
}

// Set diffuse texture
void CMesh::SetTexture(std::string texID_diff)
{
    m_pDiffTexture = CloneComponent<CTexture*>(texID_diff);
}

// Initialize Mesh
RESULT CMesh::Ready(string ID, string filePath, string fileName, eModelType type,
    string shaderID, string initSize, string meshType, string texID_Diff, string texID_Normal, _bool assimp)
{
    m_tag = ID;
    m_initSize = initSize;
    m_meshType = meshType;

    VTX* pVertices = nullptr;
    _uint* pIndices = nullptr;
    _uint vertexNum = 0;
    _uint indexNum = 0;

    if (assimp)
        Ready_Assimp(filePath, fileName);
    else
        Ready_VIBuffer(type, filePath, fileName);

    Ready_Texture_Diff(texID_Diff);
    Ready_Texture_Normal(texID_Normal);
    Ready_Shader(shaderID);

    if (nullptr != m_pShader)
        m_pShader->SetTextureInfo();

	return PK_NOERROR;
}

// Load mesh information from file
RESULT CMesh::Ready_VIBuffer(eModelType type, string filePath, string fileName)
{
    VTX* pVertices = nullptr;
    _uint* pIndices = nullptr;
    _uint vertexNum = 0;
    _uint indexNum = 0;

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

        if (next == "TextureFile")
            file >> m_textureFileName;
    }

    vec3 vMin = vec3(FLT_MAX);
    vec3 vMax = vec3(FLT_MIN);
    _int r = GetRandNum(0, 170);
    _int g = GetRandNum(0, 170);
    _int b = GetRandNum(0, 170);
    vec3 rbg = vec3(r / 255.f, g / 255.f, b / 255.f);

    pVertices = new VTX[vertexNum];
    memset(pVertices, 0, sizeof(*pVertices));
    for (_uint i = 0; i < vertexNum; ++i)
    {
        vec4& vPos = pVertices[i].vPos;
        file >> vPos.x;
        file >> vPos.y;
        file >> vPos.z;
        vPos.w = 1.f;

        switch (type)
        {
        case xyz_index:
            pVertices[i].vNormal = vec4(0.f, 1.f, 0.f, 1.f);
            break;

        case xyz_normal_index:
        case xyz_normal_color_index:
            file >> pVertices[i].vNormal.x;
            file >> pVertices[i].vNormal.y;
            file >> pVertices[i].vNormal.z;
            pVertices[i].vNormal.w = 1.f;
            break;

        case xyz_normal_texUV_index:
        case xyz_normal_texUV_index_texNum:
            file >> pVertices[i].vNormal.x;
            file >> pVertices[i].vNormal.y;
            file >> pVertices[i].vNormal.z;
            pVertices[i].vNormal.w = 1.f;
            file >> pVertices[i].vTexUV.x;
            file >> pVertices[i].vTexUV.y;
            break;
        }

        if (type == xyz_normal_color_index)
        {
            file >> pVertices[i].vColour.x; pVertices[i].vColour.x = pVertices[i].vColour.x / 255.f;
            file >> pVertices[i].vColour.y; pVertices[i].vColour.y = pVertices[i].vColour.y / 255.f;
            file >> pVertices[i].vColour.z; pVertices[i].vColour.z = pVertices[i].vColour.z / 255.f;
            file >> pVertices[i].vColour.w; pVertices[i].vColour.w = pVertices[i].vColour.w / 255.f;
        }
        else
        {
            pVertices[i].vColour.r = rbg.r;
            pVertices[i].vColour.g = rbg.g;
            pVertices[i].vColour.b = rbg.b;
            pVertices[i].vColour.a = 1.0f;
        }

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
    
    m_pBoundingBox = CBoundingBox::Create(vMin, vMax, "DebugBoxShader");

    m_iTriNum = triangleNum;
    m_pTriangles = new TRIANGLE[triangleNum];
    memset(m_pTriangles, 0, sizeof(*m_pTriangles));

    IDX* pTriangles = new IDX[triangleNum];
    memset(pTriangles, 0, sizeof(*pTriangles));
    int discard = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        file >> discard;
        file >> pTriangles[i]._0;
        file >> pTriangles[i]._1;
        file >> pTriangles[i]._2;
        if (type == xyz_normal_texUV_index_texNum)
            file >> discard;

        m_pTriangles[i].p0 = pVertices[pTriangles[i]._0].vPos;
        m_pTriangles[i].p1 = pVertices[pTriangles[i]._1].vPos;
        m_pTriangles[i].p2 = pVertices[pTriangles[i]._2].vPos;
    }
    file.close();

    indexNum = triangleNum * 3;
    pIndices = new _uint[indexNum];
    memset(pIndices, 0, sizeof(*pIndices));
    _uint indexCount = 0;
    for (_uint i = 0; i < triangleNum; ++i)
    {
        pIndices[indexCount + 0] = pTriangles[i]._0;
        pIndices[indexCount + 1] = pTriangles[i]._1;
        pIndices[indexCount + 2] = pTriangles[i]._2;

        indexCount += 3;
    }

    delete[] pTriangles;

    CreateVIBuffer(&pVertices, &pIndices, vertexNum, indexNum);
    delete[] pVertices;
    delete[] pIndices;

    return PK_NOERROR;
}

// Initialize diffuse texture
void CMesh::Ready_Texture_Diff(string texID)
{
    m_pDiffTexture = CloneComponent<CTexture*>(texID);
}

// Initialize normal texture
void CMesh::Ready_Texture_Normal(std::string texID)
{
    m_pNormalTexture = CloneComponent<CTexture*>(texID);
}

// Initialize shader
void CMesh::Ready_Shader(string shaderID)
{
    m_pShader = CloneComponent<CShader*>(shaderID);
}

RESULT CMesh::Ready_Assimp(std::string filePath, std::string fileName)
{
    stringstream ss;
    ss << filePath << fileName;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(ss.str(),
        aiProcess_Triangulate | 
        aiProcess_GenSmoothNormals | 
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        return PK_ERROR_MESHFILE_OPEN;

    if (scene->HasMeshes())
    {
        processNode(scene->mRootNode, scene);
    }

    if (scene->HasAnimations())
    {
        CAnimationData::GetInstance()->LoadAnimations(scene, &m_mapBoneInformation);
    }

    return PK_NOERROR;
}

void CMesh::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        if (mesh->HasBones())
            m_bHasBone = true;
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

void CMesh::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // Bone information
    vector<BoneData> vecBones;
    vecBones.resize(mesh->mNumVertices);
    _int boneCount = m_mapBoneInformation.size();

    for (int i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        int boneIdx = 0;
        string boneName(bone->mName.data);

        map<string, _int>::iterator iter = m_mapBoneInformation.find(boneName);
        if (iter == m_mapBoneInformation.end())
        {
            boneIdx = boneCount;
            ++boneCount;
            m_mapBoneInformation[boneName] = boneIdx;
        }
        else
            boneIdx = iter->second;

        for (int j = 0; j < bone->mNumWeights; ++j)
        {
            _float weight = bone->mWeights[j].mWeight;
            _uint vertexId = bone->mWeights[j].mVertexId;
            vecBones[vertexId].AddBoneInfo(boneIdx, weight);
        }
    }

    // Vertex information
    VTX* pVertices = nullptr;
    _uint* pIndices = nullptr;
    _uint vertexNum = 0;
    _uint indexNum = 0;

    vertexNum = mesh->mNumVertices;

    pVertices = new VTX[vertexNum];
    memset(pVertices, 0, sizeof(*pVertices));
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        pVertices[i].vPos.x = mesh->mVertices[i].x;
        pVertices[i].vPos.y = mesh->mVertices[i].y;
        pVertices[i].vPos.z = mesh->mVertices[i].z;
        pVertices[i].vPos.w = 1.f;

        if (mesh->HasNormals())
        {
            pVertices[i].vNormal.x = mesh->mNormals[i].x;
            pVertices[i].vNormal.y = mesh->mNormals[i].y;
            pVertices[i].vNormal.z = mesh->mNormals[i].z;
            pVertices[i].vNormal.w = 1.f;
        }

        if (mesh->mTextureCoords[0])
        {
            pVertices[i].vTexUV.x = mesh->mTextureCoords[0][i].x;
            pVertices[i].vTexUV.y = mesh->mTextureCoords[0][i].y;
            pVertices[i].vTangent.x = mesh->mTangents[i].x;
            pVertices[i].vTangent.y = mesh->mTangents[i].y;
            pVertices[i].vTangent.z = mesh->mTangents[i].z;
            pVertices[i].vTangent.w = 1.f;
            pVertices[i].vBinormal.x = mesh->mBitangents[i].x;
            pVertices[i].vBinormal.y = mesh->mBitangents[i].y;
            pVertices[i].vBinormal.z = mesh->mBitangents[i].z;
            pVertices[i].vBinormal.z = 1.f;
        }
        else
        {
            pVertices[i].vTexUV.x = 0.f;
            pVertices[i].vTexUV.y = 0.f;
        }

        // Bone
        BoneData bone = vecBones[i];
        pVertices[i].vBoneID.x = (_float)bone.ids[0];
        pVertices[i].vBoneID.y = (_float)bone.ids[1];
        pVertices[i].vBoneID.z = (_float)bone.ids[2];
        pVertices[i].vBoneID.w = (_float)bone.ids[3];
        pVertices[i].vBoneWeight.x = bone.weights[0];
        pVertices[i].vBoneWeight.y = bone.weights[1];
        pVertices[i].vBoneWeight.z = bone.weights[2];
        pVertices[i].vBoneWeight.w = bone.weights[3];
    }

    m_iTriNum = mesh->mNumFaces;
    indexNum = m_iTriNum * 3;
    pIndices = new _uint[indexNum];
    memset(pIndices, 0, sizeof(*pIndices));
    for (unsigned int i = 0; i < m_iTriNum; ++i)
    {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; ++j)
            pIndices[(i * 3) + j] = face.mIndices[j];
    }

    CreateVIBuffer(&pVertices, &pIndices, vertexNum, indexNum);
    delete[] pVertices;
    delete[] pIndices;

    return;
}

void CMesh::AddBoneInformation(string name, _int id)
{
}

void CMesh::CreateVIBuffer(VTX** pVertices, _uint** pIndices, _uint& vertexNum, _uint& indexNum)
{
    CVIBuffer* pBuffer = CVIBuffer::Create(vertexNum, *pVertices, indexNum, *pIndices);
    if (nullptr != pBuffer)
        m_vecVIBuffers.push_back(pBuffer);
}

//void CMesh::Ready_QuadTree(_uint depth)
//{
    //if (nullptr == m_pBoundingBox || nullptr == m_pTriangles)
    //    return;

    //m_pQuadTree = CQuadTree::Create(m_pBoundingBox->m_vMax, m_pBoundingBox->m_vMin, depth);
    //if (nullptr == m_pQuadTree)
    //    return;

    //for (_uint i = 0; i < m_iTriNum; ++i)
    //{
    //    _int hashKey = m_pQuadTree->GetHashValue(m_pTriangles[i].p0.x, m_pTriangles[i].p0.z);
    //    m_pQuadTree->AddTriangleToTreeNode(hashKey, m_pTriangles[i]);

    //    hashKey = m_pQuadTree->GetHashValue(m_pTriangles[i].p1.x, m_pTriangles[i].p1.z);
    //    m_pQuadTree->AddTriangleToTreeNode(hashKey, m_pTriangles[i]);

    //    hashKey = m_pQuadTree->GetHashValue(m_pTriangles[i].p2.x, m_pTriangles[i].p2.z);
    //    m_pQuadTree->AddTriangleToTreeNode(hashKey, m_pTriangles[i]);
    //}
//}

//void CMesh::Ready_Qctree(_uint depth)
//{
//    if (nullptr == m_pBoundingBox || nullptr == m_pTriangles)
//        return;
//
//    m_pOctree = COctree::Create(m_pBoundingBox->m_vMax, m_pBoundingBox->m_vMin, depth);
//    if (nullptr == m_pOctree)
//        return;
//
//    _uint missed = 0;
//    for (_uint i = 0; i < m_iTriNum; ++i)
//    {
//        m_pOctree->AddTriangle(m_pTriangles[i], missed);
//    }
//}

// Clone component
CComponent* CMesh::Clone()
{
    return new CMesh(*this);
}

// Create an instance
CMesh* CMesh::Create(string ID, string filePath, string fileName, eModelType type,
    string shaderID, string initSize, string meshType, string texID_Diff, string texID_Normal, _bool assimp)
{
	CMesh* pInstance = new CMesh();
	if (PK_NOERROR != pInstance->Ready(ID, filePath, fileName, type,
        shaderID, initSize, meshType,
        texID_Diff, texID_Normal, assimp))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}

