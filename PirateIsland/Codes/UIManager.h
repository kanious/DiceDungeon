#ifndef _UIMANAGER_H_
#define _UIMANAGER_H_

#include "Base.h"

namespace Engine
{
	class CScene;
}

class UIManager : public Engine::CBase
{
	SINGLETON(UIManager)

private:

private:
	explicit UIManager();
	~UIManager();
public:
	void Destroy();
	void Update(const _float& dt);
	void RenderUI();
	RESULT Ready(Engine::CScene* pScene);

public:
	_bool GetCursorIsOnTheUI();

};

#endif //_UIMANAGER_H_