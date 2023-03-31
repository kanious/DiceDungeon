#include "pch.h"
#include "..\Headers\AnimationData.h"
#include "..\Headers\Animation.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

USING(Engine)
USING(std)
SINGLETON_FUNCTION(CAnimationData)

CAnimationData::CAnimationData()
{
}

CAnimationData::~CAnimationData()
{
}

void CAnimationData::Destroy()
{
	ANIM_MAP::iterator iter;
	for (iter = m_mapAnimations.begin(); iter != m_mapAnimations.end(); ++iter)
		SafeDestroy(iter->second);
	m_mapAnimations.clear();
}

void CAnimationData::AddAnimation(string tag, CAnimation* pAnimation)
{
	ANIM_MAP::iterator iter = m_mapAnimations.find(tag);
	if (iter == m_mapAnimations.end())
		m_mapAnimations.insert(ANIM_MAP::value_type(tag, pAnimation));
}

CAnimation* CAnimationData::FindAnimation(string tag)
{
	ANIM_MAP::iterator iter = m_mapAnimations.find(tag);
	if (iter != m_mapAnimations.end())
		return iter->second;

	return nullptr;
}

void CAnimationData::LoadAnimations(const aiScene* scene, map<string, _int>* pMap)
{
	for (int i = 0; i < scene->mNumAnimations; ++i)
	{
		aiAnimation* anim = scene->mAnimations[i];
		if (nullptr == anim)
			continue;
		
		string tag(anim->mName.data);
		CAnimation* pAnim = CAnimation::Create(tag, scene, anim, pMap);
		if (nullptr != pAnim)
			AddAnimation(tag, pAnim);
	}
}


