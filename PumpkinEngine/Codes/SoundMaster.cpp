#include "pch.h"
#include "..\Headers\SoundMaster.h"
#include "..\Headers\SoundSystem.h"
#include "..\Headers\SoundInfo.h"
#include "..\Headers\ChannelGroupInfo.h"


USING(Engine)
USING(std)
SINGLETON_FUNCTION(CSoundMaster)

CSoundMaster::CSoundMaster()
	: m_pSound(nullptr)
{
}

CSoundMaster::~CSoundMaster()
{
}

void CSoundMaster::Destroy()
{
	SafeDestroy(m_pSound);
}

_uint CSoundMaster::GetSoundNumber()
{
	if (nullptr == m_pSound)
		return 0;

	return m_pSound->GetSoundNumber();
}

_uint CSoundMaster::GetSoundChannelGroupNumber()
{
	if (nullptr == m_pSound)
		return 0;

	return m_pSound->GetSoundChannelGroupNumber();
}

unordered_map<string, CSoundInfo*>* CSoundMaster::GetSoundMap()
{
	if (nullptr == m_pSound)
		return nullptr;

	return m_pSound->GetSoundMap();
}

unordered_map<string, CChannelGroupInfo*>* CSoundMaster::GetChannelMap()
{
	if (nullptr == m_pSound)
		return nullptr;

	return m_pSound->GetSoundChannelMap();
}

std::unordered_map<std::string, CDSPInfo*>* CSoundMaster::GetDSPMap()
{
	if (nullptr == m_pSound)
		return nullptr;

	return m_pSound->GetDSPMap();
}

RESULT CSoundMaster::LoadSound(string tag, string path, string channelTag)
{
	if (nullptr == m_pSound)
		return PK_ERROR_NULLPTR;

	return m_pSound->LoadSound(tag, path, channelTag, FMOD_DEFAULT);
}

RESULT CSoundMaster::LoadLoopSound(string tag, string path, string channelTag)
{
	if (nullptr == m_pSound)
		return PK_ERROR_NULLPTR;
	
	return m_pSound->LoadSound(tag, path, channelTag, FMOD_LOOP_NORMAL);
}

RESULT CSoundMaster::CreateChannelGroup(string name)
{
	if (nullptr == m_pSound)
		return PK_ERROR_NULLPTR;

	return m_pSound->CreateChannelGroup(name);
}

void CSoundMaster::PlaySampleSound(string tag)
{
	if (nullptr == m_pSound)
		return;
	
	m_pSound->PlaySampleSound(tag);
}

void CSoundMaster::StopSampleSound(string tag)
{
	if (nullptr != m_pSound)
		m_pSound->StopSampleSound(tag);
}

RESULT CSoundMaster::CreateDSPEffect(string name, _int type)
{
	if (nullptr == m_pSound)
		return PK_ERROR_NULLPTR;

	return m_pSound->CreateDSPEffect(name, (FMOD_DSP_TYPE)type);
}

RESULT CSoundMaster::AddDSPEffect(string channelName, string dspName)
{
	if (nullptr == m_pSound)
		return PK_ERROR_NULLPTR;

	return m_pSound->AddDSPEffect(channelName, dspName);
}

void CSoundMaster::Ready(int number)
{
	m_pSound = CSoundSystem::Create(number, FMOD_INIT_NORMAL);
}
