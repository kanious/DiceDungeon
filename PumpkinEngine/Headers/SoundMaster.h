#ifndef _SOUNDMASTER_H_
#define _SOUNDMASTER_H_

#include "Base.h"

NAMESPACE_BEGIN(Engine)

class CSoundSystem;
class CSoundInfo;
class CChannelGroupInfo;
class CDSPInfo;
class ENGINE_API CSoundMaster : public CBase
{
	SINGLETON(CSoundMaster)

private:
	CSoundSystem*		m_pSound;

private:
	explicit CSoundMaster();
	virtual ~CSoundMaster();
	void Destroy();

public:
	_uint GetSoundNumber();
	_uint GetSoundChannelGroupNumber();
	std::unordered_map<std::string, CSoundInfo*>* GetSoundMap();
	std::unordered_map<std::string, CChannelGroupInfo*>* GetChannelMap();
	std::unordered_map<std::string, CDSPInfo*>* GetDSPMap();

public:
	RESULT LoadSound(std::string tag, std::string path, std::string channelTag);
	RESULT LoadLoopSound(std::string tag, std::string path, std::string channelTag);
	RESULT CreateChannelGroup(std::string name);
	void PlaySampleSound(std::string tag);
	void StopSampleSound(std::string tag);
	RESULT CreateDSPEffect(std::string name, _int type);
	RESULT AddDSPEffect(std::string channelName, std::string dspName);
public:
	void Ready(int number);
};

NAMESPACE_END

#endif //_SOUNDMASTER_H_