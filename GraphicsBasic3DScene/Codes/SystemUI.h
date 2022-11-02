#ifndef _SYSTEMUI_H_
#define _SYSTEMUI_H_

#include "Base.h"

class UIManager;
class PumpkinString;

class SystemUI : public Engine::CBase
{
private:
	UIManager*			m_pUIManager;
	PumpkinString*		m_pString;

private:
	explicit SystemUI();
	~SystemUI();
public:
	void Destroy();
	void RenderUI();
	RESULT Ready();

public:
	static SystemUI* Create();
};

#endif //_SYSTEMUI_H_