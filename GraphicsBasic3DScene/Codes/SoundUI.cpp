#include "SoundUI.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "SoundSystem.h"
#include "SoundInfo.h"
#include "ChannelGroupInfo.h"
#include "DSPInfo.h"
#include "PumpkinString.h"
#include "XMLParser.h"
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>


USING(Engine)
USING(ImGui)
USING(std)

SoundUI::SoundUI()
	: m_currentSoundName(""), m_pCurrentSound(nullptr), m_pMapSound(nullptr)
	, m_currentChannelTag(""), m_pCurrentChannel(nullptr), m_pMapChannel(nullptr)
	, m_pMapDSP(nullptr)
{
	m_pString = PumpkinString::GetInstance(); m_pString->AddRefCnt();
}

SoundUI::~SoundUI()
{
}

void SoundUI::Destroy()
{
	SafeDestroy(m_pString);
	m_currentSoundName = "";
	m_pCurrentSound = nullptr;
	m_pMapSound = nullptr;
	m_currentChannelTag = "";
	m_pCurrentChannel = nullptr;
	m_pMapChannel = nullptr;
	m_pMapDSP = nullptr;
}

void SoundUI::RenderUI()
{
	SetNextWindowPos(ImVec2(COpenGLDevice::GetInstance()->GetWidthSize() - 400.f, 0.f));
	SetNextWindowSize(ImVec2(400.f, COpenGLDevice::GetInstance()->GetHeightSize()));
	stringstream ss;
	ss << m_pString->GetString("TEXT_09") << "##Pumpkins Sound Setting";
	if (Begin(ss.str().c_str(), (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		RenderSoundVersionOption();
		RenderSoundComboList();
		RenderDetailSoundInfo();
		RenderChannelGroupComboList();
		RenderDetailChannelGroupInfo();
		RenderDSPInfo();
	}
	End();
}

RESULT SoundUI::Ready()
{
	m_pMapSound = CSoundMaster::GetInstance()->GetSoundMap();
	if (nullptr != m_pMapSound && 0 < m_pMapSound->size())
	{
		SOUND_MAP::iterator iter = m_pMapSound->begin();
		stringstream ss;
		ss << iter->second->channelGroupTag;
		ss << ") ";
		ss << iter->second->name;
		m_currentSoundName = ss.str();
		m_pCurrentSound = iter->second;
	}

	m_pMapChannel = CSoundMaster::GetInstance()->GetChannelMap();
	if (nullptr != m_pMapChannel && 0 < m_pMapChannel->size())
	{
		CHANNEL_MAP::iterator iter = m_pMapChannel->begin();
		m_currentChannelTag = iter->second->tag;
		m_pCurrentChannel = iter->second;
	}
	
	m_pMapDSP = CSoundMaster::GetInstance()->GetDSPMap();

	return PK_NOERROR;
}

void SoundUI::Reset()
{
	m_pCurrentSound = nullptr;
	m_currentSoundName = "";
	m_pMapSound = nullptr;
	m_pMapChannel = nullptr;
	m_pMapDSP = nullptr;

	CSoundMaster::GetInstance()->ResetAllSoundData();
	string path = CSoundMaster::GetInstance()->GetDataPath();
	string fileName = CSoundMaster::GetInstance()->GetFileName();
	CXMLParser::GetInstance()->LoadSoundData(path, fileName);

	Ready();
}

void SoundUI::RenderSoundVersionOption()
{
	Text(m_pString->GetString("TEXT_76").c_str());
	SameLine(250.f);
	_bool bCompressed = CSoundMaster::GetInstance()->GetCompressed();
	if (bCompressed)
		Text(m_pString->GetString("TEXT_77").c_str());
	else
		Text(m_pString->GetString("TEXT_78").c_str());

	stringstream ss;
	ss << m_pString->GetString("TEXT_77") << "##CompressedVersion";
	if (Button(ss.str().c_str(), ImVec2(180.f, 0.f)))
	{
		_bool previous = CSoundMaster::GetInstance()->GetCompressed();
		if (!previous)
		{
			CSoundMaster::GetInstance()->SetCompressed(true);
			Reset();
		}
	}
	SameLine(200.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_78") << "##UncompressedVersion";
	if (Button(ss.str().c_str(), ImVec2(180.f, 0.f)))
	{
		_bool previous = CSoundMaster::GetInstance()->GetCompressed();
		if (previous)
		{
			CSoundMaster::GetInstance()->SetCompressed(false);
			Reset();
		}
	}


	Text("");
}

void SoundUI::RenderSoundComboList()
{
	if (nullptr == m_pMapSound)
		return;

	Text(m_pString->GetString("TEXT_10").c_str()); SameLine(150.f);
	SetNextItemWidth(230.f);

	if (BeginCombo("##sound_list", m_currentSoundName.c_str(), ImGuiComboFlags_None))
	{
		SOUND_MAP::iterator iter;
		for (iter = m_pMapSound->begin(); iter != m_pMapSound->end(); ++iter)
		{
			stringstream ss;
			ss << iter->second->channelGroupTag;
			ss << ") ";
			ss << iter->second->name;
			string name = ss.str();
			bool is_selected = (m_currentSoundName == name);
			if (Selectable(name.c_str(), is_selected))
			{
				m_currentSoundName = name;
				m_pCurrentSound = iter->second;
			}
		}
		EndCombo();
	}
}

void SoundUI::RenderDetailSoundInfo()
{
	if (nullptr == m_pCurrentSound)
		return;

	m_pCurrentSound->GetSoundState();
	string s;

	Text(m_pString->GetString("TEXT_11").c_str()); SameLine(150.f); Text(m_pCurrentSound->name);

	Text(m_pString->GetString("TEXT_12").c_str()); SameLine(150.f); Text(m_pCurrentSound->soundType.c_str());
	Text(m_pString->GetString("TEXT_13").c_str()); SameLine(150.f); Text(m_pCurrentSound->soundFormat.c_str());
	Text(m_pString->GetString("TEXT_14").c_str()); SameLine(150.f); s = to_string(m_pCurrentSound->channels); Text(s.c_str());
	Text(m_pString->GetString("TEXT_15").c_str()); SameLine(150.f); s = to_string(m_pCurrentSound->bits); Text(s.c_str());
	Text(m_pString->GetString("TEXT_16").c_str()); SameLine(150.f); m_pCurrentSound->loop ? Text(m_pString->GetString("TEXT_19").c_str()) : Text(m_pString->GetString("TEXT_20").c_str());
	Text(m_pString->GetString("TEXT_17").c_str()); SameLine(150.f); Text(m_pCurrentSound->channelGroupTag.c_str());
	Text(m_pString->GetString("TEXT_18").c_str()); SameLine(150.f); Text(m_pCurrentSound->GetSoundState().c_str());

	SameLine(300.f);
	stringstream ss;
	ss << m_pString->GetString("TEXT_21") << "##Play";
	if (Button(ss.str().c_str(), ImVec2(40.f, 0.f)))
	{
		CSoundMaster::GetInstance()->PlaySound(m_pCurrentSound->tag);
	}
	SameLine(345.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_22") << "##Stop";
	if (Button(ss.str().c_str(), ImVec2(40.f, 0.f)))
	{
		CSoundMaster::GetInstance()->StopSound(m_pCurrentSound->tag);
	}

	_int currentPosition = m_pCurrentSound->GetCurrentPosition();
	Text(" "); SameLine(150.f);
	SetNextItemWidth(230.f);
	if (SliderInt("##position", &currentPosition, 0, m_pCurrentSound->maxLength, "", ImGuiSliderFlags_AlwaysClamp))
	{
		m_pCurrentSound->SetCurrentPosition(currentPosition);
	}
	SameLine(220.f);

	_uint minute = currentPosition / 1000 / 60;
	_uint second = currentPosition / 1000 % 60;
	ss.str("");
	ss << setw(2) << setfill('0') << minute;
	ss << ":";
	ss << setw(2) << setfill('0') << second;

	Text(ss.str().c_str());

	SameLine(255.f);
	Text(" / ");
	SameLine(275.f);

	Text(m_pCurrentSound->strMaxLength.c_str());

	float speed = m_pCurrentSound->GetFrequency(); // kHz
	speed /= 1000;
	Text(m_pString->GetString("TEXT_23").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundFrequency", &speed, 0.0f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	speed *= 1000;
	m_pCurrentSound->SetFrequency(speed);

	CHANNEL_MAP::iterator iter = m_pMapChannel->find(m_pCurrentSound->channelGroupTag);
	if (iter != m_pMapChannel->end())
	{
		float volumn = iter->second->GetVolumn();
		volumn *= 100;
		Text(m_pString->GetString("TEXT_24").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		volumn /= 100;
		iter->second->SetVolumn(volumn);

		float pitch = iter->second->GetPitch();
		pitch *= 100;
		Text(m_pString->GetString("TEXT_25").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		pitch /= 100;
		iter->second->SetPitch(pitch);

	}
	else
	{
		float volumn = m_pCurrentSound->GetVolumn();
		volumn *= 100;
		Text(m_pString->GetString("TEXT_24").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		volumn /= 100;
		m_pCurrentSound->SetVolumn(volumn);

		float pitch = m_pCurrentSound->GetPitch();
		pitch *= 100;
		Text(m_pString->GetString("TEXT_25").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		pitch /= 100;
		m_pCurrentSound->SetPitch(pitch);
	}

	float pan = m_pCurrentSound->GetPan();
	Text(m_pString->GetString("TEXT_26").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPan", &pan, -1.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	m_pCurrentSound->SetPan(pan);
}

void SoundUI::RenderChannelGroupComboList()
{
	if (nullptr == m_pMapChannel)
		return;

	Text(" ");
	Text("=====================================================");
	Text(m_pString->GetString("TEXT_27").c_str()); SameLine(150.f);
	SetNextItemWidth(230.f);
	if (BeginCombo("##channelgroup_list", m_currentChannelTag.c_str(), ImGuiComboFlags_None))
	{
		CHANNEL_MAP::iterator iter;
		for (iter = m_pMapChannel->begin(); iter != m_pMapChannel->end(); ++iter)
		{
			bool is_selected = (m_currentChannelTag == iter->second->tag);
			if (Selectable(iter->second->tag.c_str(), is_selected))
			{
				m_currentChannelTag = iter->second->tag;
				m_pCurrentChannel = iter->second;
			}
		}
		EndCombo();
	}
}

void SoundUI::RenderDetailChannelGroupInfo()
{
	if (nullptr == m_pCurrentChannel || nullptr == m_pMapDSP)
		return;

	float volumn = m_pCurrentChannel->GetVolumn();
	volumn *= 100;
	Text(m_pString->GetString("TEXT_24").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##ChannelVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
	volumn /= 100;
	m_pCurrentChannel->SetVolumn(volumn);

	float pitch = m_pCurrentChannel->GetPitch();
	pitch *= 100;
	Text(m_pString->GetString("TEXT_25").c_str()); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##ChannelPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
	pitch /= 100;
	m_pCurrentChannel->SetPitch(pitch);

	Text(m_pString->GetString("TEXT_28").c_str());
	SameLine(160.f);
	stringstream ss;
	ss << m_pString->GetString("TEXT_29") << "##Check All";
	if (Button(ss.str().c_str(), ImVec2(100.f, 0.f)))
	{
		DSP_MAP::iterator iterDSP;
		for (iterDSP = m_pMapDSP->begin(); iterDSP != m_pMapDSP->end(); ++iterDSP)
		{
			m_pCurrentChannel->SetDSP(iterDSP->second->pDSP, iterDSP->second->type, true);
			m_pCurrentChannel->SetDspCheckAll(true);
		}
	}
	SameLine(270.f);
	ss.str("");
	ss << m_pString->GetString("TEXT_30") << "##UnCheck All";
	if (Button(ss.str().c_str(), ImVec2(100.f, 0.f)))
	{
		DSP_MAP::iterator iterDSP;
		for (iterDSP = m_pMapDSP->begin(); iterDSP != m_pMapDSP->end(); ++iterDSP)
		{
			m_pCurrentChannel->SetDSP(iterDSP->second->pDSP, iterDSP->second->type, false);
			m_pCurrentChannel->SetDspCheckAll(false);
		}
	}
}

void SoundUI::RenderDSPInfo()
{
	if (nullptr == m_pMapDSP)
		return;

	DSP_MAP::iterator iterDSP;
	for (iterDSP = m_pMapDSP->begin(); iterDSP != m_pMapDSP->end(); ++iterDSP)
	{
		_bool isOn = m_pCurrentChannel->GetDSP(iterDSP->second->type);
		Checkbox(iterDSP->second->tag.c_str(), &isOn);
		m_pCurrentChannel->SetDSP(iterDSP->second->pDSP, iterDSP->second->type, isOn);

		if (isOn)
		{
			CDSPInfo* pDSP = iterDSP->second;

			SameLine(180.f);
			stringstream ss;
			ss << m_pString->GetString("TEXT_31") << "##Set Default Value" << iterDSP->first;
			if (Button(ss.str().c_str(), ImVec2(200.f, 0.f)))
			{
				pDSP->SetDefaultValue();
			}

			_int num = pDSP->GetParameterNum();
			_float fValue = 0.f;
			_int iValue = 0;
			_bool bValue = false;
			
			for (int i = 0; i < num; ++i)
			{
				FMOD_DSP_PARAMETER_DESC* desc = pDSP->GetParameterInfo(i);
				switch (desc->type)
				{
				case FMOD_DSP_PARAMETER_TYPE_FLOAT:
					fValue = pDSP->GetDSPFloatValue(i);
					Text(" "); SameLine(50.f);
					Text(desc->name); SameLine(180.f); SetNextItemWidth(200.f); SliderFloat(desc->name, &fValue
						, desc->floatdesc.min
						, desc->floatdesc.max
						, "%.0f", ImGuiSliderFlags_AlwaysClamp);
					pDSP->SetDSPFloatValue(i, fValue);
					break;
				case FMOD_DSP_PARAMETER_TYPE_INT:
					iValue = pDSP->GetDSPIntValue(i);
					Text(" "); SameLine(50.f);
					Text(desc->name); SameLine(180.f); SetNextItemWidth(200.f); SliderInt(desc->name, &iValue
						, desc->intdesc.min
						, desc->intdesc.max
						, "%d", ImGuiSliderFlags_AlwaysClamp);
					pDSP->SetDSPIntValue(i, iValue);
					break;
				case FMOD_DSP_PARAMETER_TYPE_BOOL:
					bValue = pDSP->GetDSPBoolValue(i);
					Text(" "); SameLine(50.f);
					Text(desc->name); SameLine(180.f); Checkbox(desc->name, &bValue);
					pDSP->SetDSPBoolValue(i, bValue);
					break;
				case FMOD_DSP_PARAMETER_TYPE_DATA:
					break;
				}

			}
		}
	}
}

SoundUI* SoundUI::Create()
{
	SoundUI* pInstance = new SoundUI();
	if (PK_NOERROR != pInstance->Ready())
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
