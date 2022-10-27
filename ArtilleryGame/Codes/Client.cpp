#include "Client.h"
#include "Define.h"

#include "GameMaster.h"
#include "Timer.h"
#include "InputDevice.h"
#include "OpenGLDevice.h"
#include "ConfigurationManager.h"
#include "ComponentManager.h"
#include "BattleManager.h"
#include "ConsoleManager.h"

#include "Scene.h"
#include "Scene01.h"

#include <sstream>

Client::Client()
{
	m_pGameMaster = Engine::CGameMaster::GetInstance();
	m_pTimer = Engine::CTimer::GetInstance();
	m_pInputDevice = Engine::CInputDevice::GetInstance();
	m_pOpenGLDevice = Engine::COpenGLDevice::GetInstance();
	m_pConfigurationManager = ConfigurationManager::GetInstance();

	m_iFPS = 120;
}

Client::~Client()
{
}

void Client::Loop()
{
	_uint iFPS = 0;
	_float fCheckTime = 0.f;
	ConsoleManager::GetInstance()->WriteText();
	
	while (true)
	{
		if (nullptr == m_pGameMaster || nullptr == m_pTimer ||
			nullptr == m_pInputDevice || nullptr == m_pOpenGLDevice ||
			nullptr == m_pConfigurationManager)
			break;

		if (glfwWindowShouldClose(m_pOpenGLDevice->GetWindow()))
			break;

		if (m_pInputDevice->IsKeyDown((_int)GLFW_KEY_ESCAPE))
		{
			BattleManager::GetInstance()->SetESCInfo(true);
			break;
		}

		m_pTimer->Update();
		if (m_pTimer->IsUpdateAvailable())
		{
			glViewport(0, 0, m_pConfigurationManager->GetWidth(), m_pConfigurationManager->GetHeight());
			glClearColor(0.5, 0.5, 0.5, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_float dt = m_pTimer->GetTimeDelta();
			m_pGameMaster->Update(dt);
			m_pGameMaster->Render();

			glfwSwapBuffers(m_pOpenGLDevice->GetWindow());
			glfwPollEvents();

			iFPS++;
		}

		fCheckTime += m_pTimer->GetTimeDefault();
		if (fCheckTime >= 1.f)
		{
			stringstream ssTitle;
			ssTitle << "FPS: " << iFPS;
			glfwSetWindowTitle(m_pOpenGLDevice->GetWindow(), ssTitle.str().c_str());

			iFPS = 0;
			fCheckTime = 0.f;
		}
	}
}

void Client::Destroy()
{
	if (nullptr != m_pGameMaster)
		m_pGameMaster->Destroy();
	if (nullptr != m_pTimer)
		m_pTimer->Destroy();
	if (nullptr != m_pInputDevice)
		m_pInputDevice->Destroy();
	if (nullptr != m_pOpenGLDevice)
		m_pOpenGLDevice->Destroy();
	if (nullptr != m_pConfigurationManager)
		m_pConfigurationManager->Destroy();

	ComponentManager::GetInstance()->Destroy();
	BattleManager::GetInstance()->Destroy();
	ConsoleManager::GetInstance()->Destroy();

	delete this;
}

Client* Client::Create()
{
	Client* pInstance = new Client();
	if (PK_NOERROR != pInstance->Ready())
	{
		MSGBox("Client Create Failed!");
		delete pInstance;
		return nullptr;
	}

	return pInstance;
}

RESULT Client::Ready()
{
	RESULT result = PK_NOERROR;

	srand((unsigned int)time(NULL));

	if (nullptr != m_pOpenGLDevice)
	{
		result = m_pOpenGLDevice->CreateOpenGLWindow(1200, 800, "OpenGL Window");
		if (PK_NOERROR != result)
			return result;
	}

	if (nullptr != m_pConfigurationManager)
	{
		m_pConfigurationManager->InitialSetting();
	}

	if (nullptr != m_pTimer)
	{
		m_pTimer->SetFrameRate(m_iFPS);
	}

	if (nullptr != m_pInputDevice)
	{
		result = m_pInputDevice->SetupInputSystem(m_pOpenGLDevice->GetWindow(), GLFW_CURSOR_NORMAL);
		if (PK_NOERROR != result)
			return result;
	}

	if (nullptr != m_pGameMaster)
	{
		Engine::CScene* pScene = Scene01::Create(SCENE_STAGE01);
		m_pGameMaster->SetCurrentScene(pScene);
	}

	return PK_NOERROR;
}