#include "pch.h"
#include "..\Headers\SoundMaster.h"
#include "..\Headers\SoundSystem.h"
#include "..\Headers\SoundInfo.h"
#include "..\Headers\ChannelGroupInfo.h"


USING(Engine)
USING(std)
SINGLETON_FUNCTION(CSoundMaster)

CSoundMaster::CSoundMaster()
	: m_pSoundSystem(nullptr), m_bLoadCompressedSound(false), m_DataPath("")
{
}

CSoundMaster::~CSoundMaster()
{
}

void CSoundMaster::Destroy()
{
	SafeDestroy(m_pSoundSystem);
}

_uint CSoundMaster::GetSoundNumber()
{
	if (nullptr == m_pSoundSystem)
		return 0;

	return m_pSoundSystem->GetSoundNumber();
}

_uint CSoundMaster::GetSoundChannelGroupNumber()
{
	if (nullptr == m_pSoundSystem)
		return 0;

	return m_pSoundSystem->GetSoundChannelGroupNumber();
}

unordered_map<string, CSoundInfo*>* CSoundMaster::GetSoundMap()
{
	if (nullptr == m_pSoundSystem)
		return nullptr;

	return m_pSoundSystem->GetSoundMap();
}

unordered_map<string, CChannelGroupInfo*>* CSoundMaster::GetChannelMap()
{
	if (nullptr == m_pSoundSystem)
		return nullptr;

	return m_pSoundSystem->GetSoundChannelMap();
}

std::unordered_map<std::string, CDSPInfo*>* CSoundMaster::GetDSPMap()
{
	if (nullptr == m_pSoundSystem)
		return nullptr;

	return m_pSoundSystem->GetDSPMap();
}

RESULT CSoundMaster::LoadSound(string tag, string path, string channelTag)
{
	if (nullptr == m_pSoundSystem)
		return PK_ERROR_NULLPTR;

	return m_pSoundSystem->LoadSound(tag, path, channelTag, FMOD_DEFAULT);
}

RESULT CSoundMaster::LoadLoopSound(string tag, string path, string channelTag)
{
	if (nullptr == m_pSoundSystem)
		return PK_ERROR_NULLPTR;
	
	return m_pSoundSystem->LoadSound(tag, path, channelTag, FMOD_LOOP_NORMAL);
}

RESULT CSoundMaster::CreateChannelGroup(string name)
{
	if (nullptr == m_pSoundSystem)
		return PK_ERROR_NULLPTR;

	return m_pSoundSystem->CreateChannelGroup(name);
}

void CSoundMaster::PlaySound(string tag)
{
	if (nullptr == m_pSoundSystem)
		return;
	
	m_pSoundSystem->PlaySound(tag);
}

void CSoundMaster::StopSound(string tag)
{
	if (nullptr != m_pSoundSystem)
		m_pSoundSystem->StopSound(tag);
}

RESULT CSoundMaster::CreateDSPEffect(string name, _int type)
{
	if (nullptr == m_pSoundSystem)
		return PK_ERROR_NULLPTR;

	return m_pSoundSystem->CreateDSPEffect(name, (FMOD_DSP_TYPE)type);
}

RESULT CSoundMaster::AddDSPEffect(string channelName, string dspName)
{
	if (nullptr == m_pSoundSystem)
		return PK_ERROR_NULLPTR;

	return m_pSoundSystem->AddDSPEffect(channelName, dspName);
}

void CSoundMaster::ResetAllSoundData()
{
	SafeDestroy(m_pSoundSystem);
}

void CSoundMaster::Ready(_int number)
{
	m_pSoundSystem = CSoundSystem::Create(number, FMOD_INIT_NORMAL);
}
