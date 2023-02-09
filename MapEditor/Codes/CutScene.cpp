#include "CutScene.h"
#include "OpenGLDevice.h"
#include "Define.h"

#include <sstream>
#include <iomanip>

SINGLETON_FUNCTION(CutScene)
USING(Engine)
USING(std)

CutScene::CutScene()
{
	m_vecAnimators.clear();

	Ready();
}

CutScene::~CutScene()
{
}

void CutScene::Destroy()
{
}

RESULT CutScene::Ready()
{

	return PK_NOERROR;
}