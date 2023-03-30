#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <functional>
#include "Base.h"
#include "glm\mat4x4.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

NAMESPACE_BEGIN(Engine)

// Class containing animation data
class ENGINE_API CAnimation : public CBase
{
private:
	std::string						m_tag;

	std::vector<glm::mat4x4>		m_vecMatrix;
	typedef std::vector<std::vector<glm::mat4x4>*> BONE_MATRIX;
	BONE_MATRIX						m_vecBoneMatrix;
	_uint							m_iDuration;
	_float							m_fTickPerSec;

private:
	explicit CAnimation();
	virtual ~CAnimation();

private:
	virtual void Destroy();

public:
	void FrameMove(_float& curTime, _uint& frameIdx, _bool reverse, std::function<void(void)> callback);
	std::vector<glm::mat4x4>* GetMatrix(_uint iFrameIndex);
	_uint GetAnimationLength()			{ return m_iDuration; }
	std::string GetName()				{ return m_tag; }

private:
	RESULT Ready(std::string tag, std::string filePath);
	RESULT Ready(std::string tag, const aiScene* scene, const aiAnimation* animation, std::map<std::string, _int>* pMap);
	void CheckHierarchy(const aiNode* node, std::vector<glm::mat4x4>* boneVec, std::map<std::string, _int>* pMap, glm::mat4x4 parentMat,
		std::vector<glm::mat4x4>& boneOffsets, glm::mat4x4& matGlobal);
	void CastToGLM(const aiMatrix4x4& in, glm::mat4x4& out);
public:
	static CAnimation* Create(std::string tag, std::string filePath);
	static CAnimation* Create(std::string tag, const aiScene* scene, const aiAnimation* animation, std::map<std::string, _int>* pMap);
};

NAMESPACE_END

#endif //_ANIMATION_H_