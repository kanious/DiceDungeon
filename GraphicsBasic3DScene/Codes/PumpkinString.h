#ifndef _PUMPKINSTRING_H_
#define _PUMPKINSTRING_H_

#include "Base.h"

class PumpkinString : public Engine::CBase
{
	SINGLETON(PumpkinString)

public:
	enum eLanguageType
	{
		lang_en = 0,
		lang_sp,
		lang_ko,
		lang_jp,
		lang_ru,
		lang_gr,
		lang_th,
		lang_vt,
		lang_end
	};

private:
	typedef std::unordered_map<std::string, std::string>	STRING_MAP;
	STRING_MAP						m_mapString;

	eLanguageType					m_eType;
	std::string						m_DataPath;
	std::string						m_FileName[lang_end];

private:
	explicit PumpkinString();
	~PumpkinString();
public:
	void Destroy();
	RESULT Ready();

public:
	void LanguageChange(eLanguageType type);
	std::string GetString(std::string key);

private:
	void LoadStrings(eLanguageType type);
};

#endif //_PUMPKINSTRING_H_