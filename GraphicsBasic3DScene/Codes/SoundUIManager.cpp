#include "SoundUIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "SoundSystem.h"
#include "SoundInfo.h"
#include "ChannelGroupInfo.h"
#include "DSPInfo.h"
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>


USING(Engine)
USING(ImGui)
USING(std)

SoundUIManager::SoundUIManager()
	: m_currentSoundName(""), m_pCurrentSound(nullptr), m_pMapSound(nullptr)
	, m_currentChannelTag(""), m_pCurrentChannel(nullptr), m_pMapChannel(nullptr)
	, m_pMapDSP(nullptr)
{
}

SoundUIManager::~SoundUIManager()
{
}

void SoundUIManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

void SoundUIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	SetNextWindowPos(ImVec2(800.f, 0.f));
	SetNextWindowSize(ImVec2(400.f, COpenGLDevice::GetInstance()->GetHeightSize()));
	if (Begin("Pumpkins Sound Setting", (bool*)0, ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		RenderSoundComboList();
		RenderDetailSoundInfo();
		RenderChannelGroupComboList();
		RenderDetailChannelGroupInfo();
		RenderDSPInfo();
	}
	End();

	Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

RESULT SoundUIManager::Ready()
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

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

void SoundUIManager::RenderSoundComboList()
{
	if (nullptr == m_pMapSound)
		return;

	Text("* Sounds"); SameLine(150.f);
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

void SoundUIManager::RenderDetailSoundInfo()
{
	if (nullptr == m_pCurrentSound)
		return;

	m_pCurrentSound->GetSoundState();
	string s;

	Text(" FileName"); SameLine(150.f); Text(m_pCurrentSound->name);

	Text(" Type"); SameLine(150.f); Text(m_pCurrentSound->soundType.c_str());
	Text(" Format"); SameLine(150.f); Text(m_pCurrentSound->soundFormat.c_str());
	Text(" Channels"); SameLine(150.f); s = to_string(m_pCurrentSound->channels); Text(s.c_str());
	Text(" Bits"); SameLine(150.f); s = to_string(m_pCurrentSound->bits); Text(s.c_str());
	Text(" Loop"); SameLine(150.f); m_pCurrentSound->loop ? Text("Yes") : Text("No");
	Text(" ChannelGroup"); SameLine(150.f); Text(m_pCurrentSound->channelGroupTag.c_str());
	Text(" State"); SameLine(150.f); Text(m_pCurrentSound->GetSoundState().c_str());

	SameLine(300.f);
	if (Button("Play", ImVec2(40.f, 0.f)))
	{
		CSoundMaster::GetInstance()->PlaySampleSound(m_pCurrentSound->tag);
	}
	SameLine(345.f);
	if (Button("Stop", ImVec2(40.f, 0.f)))
	{
		CSoundMaster::GetInstance()->StopSampleSound(m_pCurrentSound->tag);
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
	stringstream ss;
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
	Text(" Frequency"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundFrequency", &speed, 0.0f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	speed *= 1000;
	m_pCurrentSound->SetFrequency(speed);

	CHANNEL_MAP::iterator iter = m_pMapChannel->find(m_pCurrentSound->channelGroupTag);
	if (iter != m_pMapChannel->end())
	{
		float volumn = iter->second->GetVolumn();
		volumn *= 100;
		Text(" Volumn"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		volumn /= 100;
		iter->second->SetVolumn(volumn);

		float pitch = iter->second->GetPitch();
		pitch *= 100;
		Text(" Pitch"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		pitch /= 100;
		iter->second->SetPitch(pitch);

	}
	else
	{
		float volumn = m_pCurrentSound->GetVolumn();
		volumn *= 100;
		Text(" Volumn"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		volumn /= 100;
		m_pCurrentSound->SetVolumn(volumn);

		float pitch = m_pCurrentSound->GetPitch();
		pitch *= 100;
		Text(" Pitch"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
		pitch /= 100;
		m_pCurrentSound->SetPitch(pitch);
	}

	float pan = m_pCurrentSound->GetPan();
	Text(" Pan"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##SoundPan", &pan, -1.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	m_pCurrentSound->SetPan(pan);
}

void SoundUIManager::RenderChannelGroupComboList()
{
	if (nullptr == m_pMapChannel)
		return;

	Text(" ");
	Text("=====================================================");
	Text("* ChannelGroups"); SameLine(150.f);
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

void SoundUIManager::RenderDetailChannelGroupInfo()
{
	if (nullptr == m_pCurrentChannel || nullptr == m_pMapDSP)
		return;

	float volumn = m_pCurrentChannel->GetVolumn();
	volumn *= 100;
	Text(" Volumn"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##ChannelVolume", &volumn, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
	volumn /= 100;
	m_pCurrentChannel->SetVolumn(volumn);

	float pitch = m_pCurrentChannel->GetPitch();
	pitch *= 100;
	Text(" Pitch"); SameLine(150.f); SetNextItemWidth(230.f); SliderFloat("##ChannelPitch", &pitch, 0.0f, 200.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
	pitch /= 100;
	m_pCurrentChannel->SetPitch(pitch);

	Text(" DSPs");
	SameLine(160.f);
	if (Button("Check All", ImVec2(100.f, 0.f)))
	{
		DSP_MAP::iterator iterDSP;
		for (iterDSP = m_pMapDSP->begin(); iterDSP != m_pMapDSP->end(); ++iterDSP)
		{
			m_pCurrentChannel->SetDSP(iterDSP->second->pDSP, iterDSP->second->type, true);
			m_pCurrentChannel->SetDspCheckAll(true);
		}
	}
	SameLine(270.f);
	if (Button("UnCheck All", ImVec2(100.f, 0.f)))
	{
		DSP_MAP::iterator iterDSP;
		for (iterDSP = m_pMapDSP->begin(); iterDSP != m_pMapDSP->end(); ++iterDSP)
		{
			m_pCurrentChannel->SetDSP(iterDSP->second->pDSP, iterDSP->second->type, false);
			m_pCurrentChannel->SetDspCheckAll(false);
		}
	}
}

void SoundUIManager::RenderDSPInfo()
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
			if (Button("Set Default Value", ImVec2(200.f, 0.f)))
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
