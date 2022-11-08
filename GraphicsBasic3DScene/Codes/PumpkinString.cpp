#include "PumpkinString.h"
#include "XMLParser.h"
#include <sstream>
#include <atlconv.h>


SINGLETON_FUNCTION(PumpkinString)
USING(Engine)
USING(std)

PumpkinString::PumpkinString()
	: m_eType(lang_en)
{
	m_mapString.clear();

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));
	stringstream ss;
	ss << str << "\\";

	m_DataPath = ss.str();
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

	CXMLParser::GetInstance()->LoadLanguageData(m_DataPath, m_FileName[type], m_mapString);
}
