#ifndef _CONSOLEMANAGER_H_
#define _CONSOLEMANAGER_H_

#include "Define.h"
#include "OpenGLDefines.h"

class Enemy;
class Barrel;
class ConsoleManager
{
	SINGLETON(ConsoleManager)

private:
	vector<string>				m_vecString;
	list<eCONSOLE_COLOR>		m_listColor;
	list<string>				m_listString;
	string						m_strWeaponType;

private:
	explicit ConsoleManager();
	~ConsoleManager();
public:
	void Destroy();

public:
	void AddEnemyHitText(_int, _int, _float);
	void AddEnemyMissText(_int, _int, _float);
	void AddWinText(_uint);
	void ChangeBulletType(eBULLET_TYPE);
	void WriteText();
	void ResetMessages();
private:
	void AddText(eCONSOLE_COLOR, string);
	void InitText();
	void SetColor(eCONSOLE_COLOR);
};

#endif //_CONSOLEMANAGER_H_