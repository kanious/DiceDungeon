#include <string>
#include "ConsoleManager.h"
#include "Enemy.h"


SINGLETON_FUNCTION(ConsoleManager)

ConsoleManager::ConsoleManager()
{
	m_vecString.clear();
	m_listColor.clear();
	m_listString.clear();
	m_strWeaponType = "";

	InitText();
}

ConsoleManager::~ConsoleManager()
{
}

void ConsoleManager::Destroy()
{
	DestroyInstance();
}

void ConsoleManager::AddEnemyHitText(_int currentHP, _int maxHP, _float distance)
{
	string str = " [  Hit  ] Enemy is hit!\t(";
	str.append(to_string(currentHP));
	str.append("/");
	str.append(to_string(maxHP));
	str.append(") distance: ");
	str.append(to_string(distance));
	str.append("m");

	AddText(CONSOLE_SKYBLUE, str);
}

void ConsoleManager::AddEnemyMissText(_int currentHP, _int maxHP, _float distance)
{
	string str = " [  Miss ] Missed\t\t(";
	str.append(to_string(currentHP));
	str.append("/");
	str.append(to_string(maxHP));
	str.append(") distance: ");
	str.append(to_string(distance));
	str.append("m");

	AddText(CONSOLE_RED, str);
}

void ConsoleManager::AddWinText(_uint bulletCount)
{
	string str = " \t�١ڡ١١ڡ١١ڡ�";
	AddText(CONSOLE_YELLOW, str);

	str = " \t�١ڡ�  WIN  �١ڡ�";
	AddText(CONSOLE_YELLOW, str);

	str = " \t�١ڡ١١ڡ١١ڡ�";
	AddText(CONSOLE_YELLOW, str);

	str = " Launched Bullets Count: ";
	str.append(to_string(bulletCount));
	AddText(CONSOLE_GREEN, str);

	str = " You won a stage. Thank you for playing! Please press [N] to start new game.";
	AddText(CONSOLE_GREEN, str);
}

void ConsoleManager::ChangeBulletType(eBULLET_TYPE type)
{
	m_strWeaponType = " [ Current Bullet ] ";
	switch (type)
	{
	case B_BOUNCE:
		m_strWeaponType.append("1. Bounce");
		break;
	case B_DIVIDED:
		m_strWeaponType.append("2. Divided");
		break;
	case B_MULTIPLE:
		m_strWeaponType.append("3. Multiple");
		break;
	case B_BOMBING:
		m_strWeaponType.append("4. Bombing");
		break;
	case B_ROLLING:
		m_strWeaponType.append("5. Rolling");
		break;
	}
	m_strWeaponType.append(" Bullet");
	WriteText();
}

void ConsoleManager::WriteText()
{
	system("cls");
	vector<string>::iterator iter;
	for (iter = m_vecString.begin(); iter != m_vecString.end(); ++iter)
		cout << *iter << endl;

	cout << m_strWeaponType << endl;

	list<eCONSOLE_COLOR>::iterator iterColor = m_listColor.begin();
	list<string>::iterator iterString;
	for (iterString = m_listString.begin(); iterString != m_listString.end(); ++iterString)
	{
		SetColor(*iterColor);
		cout << *iterString << endl;
		++iterColor;
	}
	SetColor(CONSOLE_DEFAULT);
}

void ConsoleManager::ResetMessages()
{
	m_listColor.clear();
	m_listString.clear();
	ChangeBulletType(B_BOUNCE);
}

void ConsoleManager::AddText(eCONSOLE_COLOR eColor, string str)
{
	m_listColor.push_back(eColor);
	m_listString.push_back(str);

	if (m_listColor.size() > 15)
	{
		m_listColor.erase(m_listColor.begin());
		m_listString.erase(m_listString.begin());
	}
}

void ConsoleManager::InitText()
{
	string str =  "*************************************************************************"; m_vecString.push_back(str);
	string str2 = "* [1] Bounce Bullet\t[Space] Fire Bullet\t[F1] Normal view\t*"; m_vecString.push_back(str2);
	string str3 = "* [2] Divided Bullet\t\t\t\t[F2] Top View\t\t*"; m_vecString.push_back(str3);
	string str4 = "* [3] Multiple Bullet\t[N] New Game\t\t[F3] Front View\t\t*"; m_vecString.push_back(str4);
	string str5 = "* [4] Bombing Bullet\t\t\t\t[F4] Right view\t\t*"; m_vecString.push_back(str5);
	string str6 = "* [5] Bounce Bullet\t[Backspace] Game close\t\t\t\t*"; m_vecString.push_back(str6);
	string str7 = "*************************************************************************"; m_vecString.push_back(str7);
	ChangeBulletType(B_BOUNCE);
}

void ConsoleManager::SetColor(eCONSOLE_COLOR eColor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (_int)eColor);
}
