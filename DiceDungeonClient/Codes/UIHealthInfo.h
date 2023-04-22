#ifndef _UIHEALTHINFO_H_
#define _UIHEALTHINFO_H_

#include "Base.h"
#include "glm\vec3.hpp"

namespace Engine
{
	class CGameObject;
}

// Health Bar UI
class UIHealthInfo : public Engine::CBase
{
private:
	Engine::CGameObject*	m_pObj;
	std::string				m_name;
	std::string				m_windowName;
	static _int				g_Num;

private:
	explicit UIHealthInfo();
	virtual ~UIHealthInfo();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Basic Render Function
	void RenderUI();

private:
	// Initialize
	RESULT Ready(Engine::CGameObject* pObj);
public:
	// Create the instance
	static UIHealthInfo* Create(Engine::CGameObject* pObj);
};

#endif //_UIHEALTHINFO_H_