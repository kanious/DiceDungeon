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
	CSoundSystem*		m_pSoundSystem;
	_bool				m_bLoadCompressedSound;
	std::string			m_DataPath;

private:
	explicit CSoundMaster();
	virtual ~CSoundMaster();
	void Destroy();

public:
	_uint GetSoundNumber();
	_uint GetSoundChannelGroupNumber();
	_bool GetCompressed()					{ return m_bLoadCompressedSound; }
	std::string GetDataPath()				{ return m_DataPath; }
	std::unordered_map<std::string, CSoundInfo*>* GetSoundMap();
	std::unordered_map<std::string, CChannelGroupInfo*>* GetChannelMap();
	std::unordered_map<std::string, CDSPInfo*>* GetDSPMap();
	void SetCompressed(_bool compressed)	{ m_bLoadCompressedSound = compressed; }
	void SetDataPath(std::string path)		{ m_DataPath = path; }

public:
	RESULT LoadSound(std::string tag, std::string path, std::string channelTag);
	RESULT LoadLoopSound(std::string tag, std::string path, std::string channelTag);
	RESULT CreateChannelGroup(std::string name);
	void PlaySound(std::string tag);
	void StopSound(std::string tag);
	RESULT CreateDSPEffect(std::string name, _int type);
	RESULT AddDSPEffect(std::string channelName, std::string dspName);
	void ResetAllSoundData();
public:
	void Ready(_int number);
};

NAMESPACE_END

#endif //_SOUNDMASTER_H_