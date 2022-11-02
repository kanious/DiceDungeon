#ifndef _SOUND_H_
#define _SOUND_H_

#include "Base.h"

#include "fmod\fmod.hpp"
#include "fmod\fmod_dsp.h"
#include "fmod\fmod_errors.h"

NAMESPACE_BEGIN(Engine)

class CSoundInfo;
class CChannelGroupInfo;
class CDSPInfo;
class ENGINE_API CSoundSystem : public CBase
{
	//SINGLETON(CSoundSystem)

private:
	FMOD::System*					m_pSystem;

	typedef std::unordered_map<std::string, CSoundInfo*> SOUND_MAP;
	SOUND_MAP m_mapSounds;

	typedef std::unordered_map<std::string, CChannelGroupInfo*> CHANNEL_MAP;
	CHANNEL_MAP m_mapChannels;

	typedef std::unordered_map<std::string, CDSPInfo*> DSP_MAP;
	DSP_MAP m_mapDSPs;

private:
	explicit CSoundSystem();
	virtual ~CSoundSystem();
	void Destroy();

public:
	_uint GetSoundNumber()						{ return (_uint)m_mapSounds.size(); }
	_uint GetSoundChannelGroupNumber()			{ return (_uint)m_mapChannels.size(); }
	SOUND_MAP* GetSoundMap()					{ return &m_mapSounds; }
	CHANNEL_MAP* GetSoundChannelMap()			{ return &m_mapChannels; }
	DSP_MAP* GetDSPMap()						{ return &m_mapDSPs; }
public:
	RESULT LoadSound(std::string tag, std::string path, std::string channelTag, _int mode);
	RESULT CreateChannelGroup(std::string tag);
	void PlayBGMSound();
	void PlaySound(std::string tag);
	void StopSound(std::string tag);
	RESULT CreateDSPEffect(std::string name, FMOD_DSP_TYPE type);
	RESULT AddDSPEffect(std::string channelTag, std::string dspName);

private:
	RESULT ErrorCheck(FMOD_RESULT result);
	RESULT Ready(_int number, _int flag);
public:
	static CSoundSystem* Create(_int number, _int flag);
};

NAMESPACE_END

#endif //_SOUND_H_