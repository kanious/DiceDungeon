#ifndef _SOUNDUI_H_
#define _SOUNDUI_H_

#include "Base.h"

namespace Engine
{
	class CSoundInfo;
	class CChannelGroupInfo;
	class CDSPInfo;
}
class PumpkinString;

class SoundUI : public Engine::CBase
{
private:
	std::string						m_currentSoundName;
	Engine::CSoundInfo*				m_pCurrentSound;
	PumpkinString*					m_pString;
	_bool							m_bCompressed;

	typedef std::unordered_map<std::string, Engine::CSoundInfo*> SOUND_MAP;
	SOUND_MAP*						m_pMapSound;

	std::string						m_currentChannelTag;
	Engine::CChannelGroupInfo*		m_pCurrentChannel;
	typedef std::unordered_map<std::string, Engine::CChannelGroupInfo*> CHANNEL_MAP;
	CHANNEL_MAP*					m_pMapChannel;

	typedef std::unordered_map<std::string, Engine::CDSPInfo*> DSP_MAP;
	DSP_MAP*						m_pMapDSP;

private:
	explicit SoundUI();
	~SoundUI();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready();
	void Reset();

private:
	void RenderSoundVersionOption();
	void RenderSoundComboList();
	void RenderDetailSoundInfo();
	void RenderChannelGroupComboList();
	void RenderDetailChannelGroupInfo();
	void RenderDSPInfo();

public:
	static SoundUI* Create();
};

#endif //_SOUNDUI_H_