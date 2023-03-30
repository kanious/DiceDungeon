#include "pch.h"
#include "..\Headers\Animation.h"
#include "..\Headers\OpenGLDefines.h"
#include <windows.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


USING(Engine)
USING(std)
USING(glm)

CAnimation::CAnimation()
    : m_tag(""), m_iDuration(0), m_fTickPerSec(0.f)
{
    m_vecMatrix.clear();
}

CAnimation::~CAnimation()
{
}

void CAnimation::Destroy()
{
    m_vecMatrix.clear();

    for (int i = 0; i < m_vecBoneMatrix.size(); ++i)
    {
        delete m_vecBoneMatrix[i];
    }
    m_vecBoneMatrix.clear();
}

void CAnimation::FrameMove(_float& curTime, _uint& frameIdx, _bool reverse, function<void(void)> callback)
{
    if (curTime >= 1.f / m_fTickPerSec)
    {
        curTime = 0.f;
        if (!reverse)
        {
            if (frameIdx >= m_vecBoneMatrix.size() - 1)
            {
                frameIdx = 0;
                callback();
            }
            else
                ++frameIdx;
        }
        else
        {
            if (0 == frameIdx)
            {
                frameIdx = m_vecBoneMatrix.size() - 1;
                callback();
            }
            else
                --frameIdx;
        }
    }
}

vector<mat4x4>* CAnimation::GetMatrix(_uint iFrameIndex)
{
    return m_vecBoneMatrix[iFrameIndex];
}

RESULT CAnimation::Ready(string tag, string filePath)
{
    m_tag = tag;

    wstring wstr = wstring(filePath.begin(), filePath.end());
    HANDLE hFile = CreateFile(wstr.c_str()
        , GENERIC_READ
        , 0
        , NULL
        , OPEN_EXISTING
        , FILE_ATTRIBUTE_NORMAL
        , NULL);
    unsigned long dwByte;

    bool success = false;
    success = ReadFile(hFile, &m_iDuration, sizeof(_uint), &dwByte, NULL);
    success = ReadFile(hFile, &m_fTickPerSec, sizeof(_float), &dwByte, NULL);

    for (int i = 0; i < m_iDuration; ++i)
    {
        mat4x4 mat(1.f);
        success = ReadFile(hFile, &mat, sizeof(mat4x4), &dwByte, NULL);
        m_vecMatrix.push_back(mat);
    }

    return PK_NOERROR;
}

RESULT CAnimation::Ready(string tag, const aiScene* scene, const aiAnimation* animation, map<string, _int>* pMap)
{
    m_tag = tag;

    m_fTickPerSec = (_float)animation->mTicksPerSecond;
    m_iDuration = (_uint)animation->mDuration;

    vec3 vPos = vec3(0.f);
    quat qRot = quat(vec3(0.f));
    vec3 vScale = vec3(0.f);

    aiMesh* mesh = scene->mMeshes[0];
    vector<mat4x4> vecBoneOffsets;
    vecBoneOffsets.resize(mesh->mNumBones);
    for (int i = 0; i < mesh->mNumBones; ++i)
    {
        aiBone* bone = mesh->mBones[i];
        string boneName(bone->mName.data);
        map<string, _int>::iterator iter = pMap->find(boneName);
        if (iter == pMap->end())
            continue;
        _int boneId = iter->second;
        CastToGLM(bone->mOffsetMatrix, vecBoneOffsets[boneId]);
    }

    aiMatrix4x4 g = scene->mRootNode->mTransformation;
    aiMatrix4x4 inverse = g.Inverse();
    mat4x4 matGlobal;
    CastToGLM(inverse, matGlobal);

    m_vecBoneMatrix.resize(m_iDuration);
    for (int i = 0; i < m_iDuration; ++i)
    {
        vector<mat4x4>* pBoneVec = new vector<mat4x4>();
        pBoneVec->resize(animation->mNumChannels);

        for (int j = 0; j < animation->mNumChannels; ++j)
        {
            const aiNodeAnim* nodeAnim = animation->mChannels[j];
            _uint numKeys = nodeAnim->mNumPositionKeys;
            string boneName(nodeAnim->mNodeName.data);
            map<string, _int>::iterator iter = pMap->find(boneName);
            if (iter == pMap->end())
                continue;
            _int boneId = iter->second;

            // position
            for (int k = 0; k < numKeys; ++k)
            {
                if (i >= nodeAnim->mPositionKeys[k].mTime)
                {
                    vPos.x = nodeAnim->mPositionKeys[k].mValue.x;
                    vPos.y = nodeAnim->mPositionKeys[k].mValue.y;
                    vPos.z = nodeAnim->mPositionKeys[k].mValue.z;
                }
                else
                    break;
            }

            // rotation
            for (int k = 0; k < numKeys; ++k)
            {
                if (i >= nodeAnim->mPositionKeys[k].mTime)
                {
                    qRot.x = nodeAnim->mRotationKeys[k].mValue.x;
                    qRot.y = nodeAnim->mRotationKeys[k].mValue.y;
                    qRot.z = nodeAnim->mRotationKeys[k].mValue.z;
                    qRot.w = nodeAnim->mRotationKeys[k].mValue.w;
                }
                else
                    break;
            }
            
            // scale
            for (int k = 0; k < numKeys; ++k)
            {
                if (i >= nodeAnim->mScalingKeys[k].mTime)
                {
                    vScale.x = nodeAnim->mScalingKeys[k].mValue.x;
                    vScale.y = nodeAnim->mScalingKeys[k].mValue.y;
                    vScale.z = nodeAnim->mScalingKeys[k].mValue.z;
                }
                else
                    break;
            }

            mat4x4 translationMatrix = translate(mat4x4(1.f), vPos);
            mat4x4 rotationMatrix = mat4_cast(qRot);
            mat4x4 scaleMatrix = scale(mat4x4(1.0f), vScale);

            mat4x4 transform = translationMatrix * rotationMatrix * scaleMatrix;
            (*pBoneVec)[boneId] = transform;
        }

		mat4x4 identity(1.f);
        CheckHierarchy(scene->mRootNode, pBoneVec, pMap, identity, vecBoneOffsets, matGlobal);

        m_vecBoneMatrix[i] = pBoneVec;
    }

    return PK_NOERROR;
}

void CAnimation::CheckHierarchy(const aiNode* node, vector<mat4x4>* boneVec, map<string, _int>* pMap, mat4x4 parentMat,
    vector<mat4x4>& boneOffsets, mat4x4& matGlobal)
{
    string nodeName(node->mName.data);
    mat4x4 transformationMatrix(1.f);
    CastToGLM(node->mTransformation, transformationMatrix);

    mat4x4 boneOffset(1.f);
    mat4x4 globalTransformation = parentMat * transformationMatrix;

    map<string, _int>::iterator iter = pMap->find(nodeName);
    _int boneId = -1;
    if (iter != pMap->end())
    {
        boneId = iter->second;
        transformationMatrix = (*boneVec)[boneId];
        globalTransformation = parentMat * transformationMatrix;
        boneOffset = boneOffsets[boneId];

        (*boneVec)[boneId] = matGlobal * globalTransformation * boneOffset;
    }

    for (int i = 0; i < node->mNumChildren; ++i)
    {
        CheckHierarchy(node->mChildren[i], boneVec, pMap, globalTransformation, boneOffsets, matGlobal);
    }
}

void CAnimation::CastToGLM(const aiMatrix4x4& in, mat4x4& out)
{
    out = mat4x4(in.a1, in.b1, in.c1, in.d1,
		in.a2, in.b2, in.c2, in.d2,
		in.a3, in.b3, in.c3, in.d3,
		in.a4, in.b4, in.c4, in.d4);
}

CAnimation* CAnimation::Create(string tag, string filePath)
{
    CAnimation* pInstance = new CAnimation();
    if (PK_NOERROR != pInstance->Ready(tag, filePath))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}

CAnimation* CAnimation::Create(std::string tag, const aiScene* scene, const aiAnimation* animation, map<string, _int>* pMap)
{
    CAnimation* pInstance = new CAnimation();
    if (PK_NOERROR != pInstance->Ready(tag, scene, animation, pMap))
    {
        pInstance->Destroy();
        pInstance = nullptr;
    }

    return pInstance;
}
