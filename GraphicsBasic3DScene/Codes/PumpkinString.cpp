#include "PumpkinString.h"
#include "XMLParser.h"
#include <sstream>


SINGLETON_FUNCTION(PumpkinString)
USING(Engine)
USING(std)

PumpkinString::PumpkinString()
	: m_eType(lang_en)
{
	m_mapString.clear();
	m_DataPath = "Assets\\xmlData\\lang\\";
	m_FileName[lang_en] = "lang_en.xml";
	m_FileName[lang_sp] = "lang_sp.xml";
	m_FileName[lang_ko] = "lang_ko.xml";
	m_FileName[lang_jp] = "lang_jp.xml";
	m_FileName[lang_ru] = "lang_ru.xml";
	m_FileName[lang_gr] = "lang_gr.xml";
	m_FileName[lang_th] = "lang_th.xml";
	m_FileName[lang_vt] = "lang_vt.xml";
}

PumpkinString::~PumpkinString()
{
}

void PumpkinString::Destroy()
{
	m_mapString.clear();
}

RESULT PumpkinString::Ready()
{
	LoadStrings(m_eType);

	return PK_NOERROR;
}

void PumpkinString::LanguageChange(eLanguageType type)
{
	if (type == m_eType)
		return;

	m_eType = type;
	LoadStrings(type);
}

std::string PumpkinString::GetString(std::string key)
{
	string str = key;

	STRING_MAP::iterator iter = m_mapString.find(key);
	if (iter != m_mapString.end())
		str = iter->second;

	return str;
}

void PumpkinString::LoadStrings(eLanguageType type)
{
	m_mapString.clear();

	stringstream ss;
	ss << m_DataPath << m_FileName[type];
	CXMLParser::GetInstance()->LoadLanguageData(ss.str(), m_mapString);
}
