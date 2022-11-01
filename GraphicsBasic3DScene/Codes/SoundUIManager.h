#ifndef _SOUNDUIMANAGER_H_
#define _SOUNDUIMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CSoundInfo;
	class CChannelGroupInfo;
	class CDSPInfo;
}

class SoundUIManager : public Engine::CBase
{
private:
	std::string						m_currentSoundName;
	Engine::CSoundInfo*				m_pCurrentSound;
	typedef std::unordered_map<std::string, Engine::CSoundInfo*> SOUND_MAP;
	SOUND_MAP*						m_pMapSound;

	std::string						m_currentChannelTag;
	Engine::CChannelGroupInfo*		m_pCurrentChannel;
	typedef std::unordered_map<std::string, Engine::CChannelGroupInfo*> CHANNEL_MAP;
	CHANNEL_MAP*					m_pMapChannel;

	typedef std::unordered_map<std::string, Engine::CDSPInfo*> DSP_MAP;
	DSP_MAP*						m_pMapDSP;

private:
	explicit SoundUIManager();
	~SoundUIManager();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready();

private:
	void RenderSoundComboList();
	void RenderDetailSoundInfo();
	void RenderChannelGroupComboList();
	void RenderDetailChannelGroupInfo();
	void RenderDSPInfo();
};

#endif //_SOUNDUIMANAGER_H_