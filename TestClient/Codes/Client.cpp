#include "Client.h"
#include "Define.h"

#include "GameMaster.h"
#include "Timer.h"
#include "InputDevice.h"
#include "OpenGLDevice.h"
#include "ConfigurationManager.h"

#include "Scene.h"
#include "Scene01.h"

//#include <stdio.h>
//#include <stdlib.h>
#include <sstream>
#include "Cube.h"
#include "Shaders_remove.h"

Client::Client()
{
	m_pGameMaster = Engine::CGameMaster::GetInstance();
	m_pTimer = Engine::CTimer::GetInstance();
	m_pInputDevice = Engine::CInputDevice::GetInstance();
	m_pOpenGLDevice = Engine::COpenGLDevice::GetInstance();
	m_pConfigurationManager = ConfigurationManager::GetInstance();

	m_iFPS = 60;
}

Client::~Client()
{
}

void Client::Loop()
{
	_uint iFPS = 0;
	_float fCheckTime = 0.f;

	while (true)
	{
		if (nullptr == m_pGameMaster || nullptr == m_pTimer ||
			nullptr == m_pInputDevice || nullptr == m_pOpenGLDevice ||
			nullptr == m_pConfigurationManager)
			break;

		// 키 눌러서 종료 or 윈도우 종료 캐치
		if (glfwWindowShouldClose(m_pOpenGLDevice->GetWindow()))
			break;

		if (m_pInputDevice->IsKeyDown((_int)GLFW_KEY_ESCAPE))
			break;
		//
		// main에서 메모리릭 관련 코드 넣기
		// 
		// 
		// Mesh 파일에서 불러와서 뿌리기
		// 
		// 
		// 
		// TestClient 만들어놓은거
		// 그거 복사해서 Atilliery Project 만들어서 적용하기
		// 그리고 Physics 시작
		// 최소한 이번 주 토요일까지는 엔진 완료해야 됨
		// Physics도 스프링 이런건 하나도 모르겠어서 공부 해야돼
		// 점프 같은건 금방 하긴 하는데...
		// 게임 꾸미는 시간 (추가적인 Physics 기능 넣을 수 있음 넣기)
		// 
		// Unique Bullet 5개 <- 이 부분이 ★어필 포인트★!!!
		// idea 1. 추적 총알 (특정 포물선을 그리면서 날라감, 가장 가까운 적 기준)
		// idea 2. 도착지에서 폭발하여 방사형으로 범위 피해 (발사된 방향으로)
		// idea 3. 블랙홀 총알 (범위형 피해 + 적들을 착탄지점으로 살짝 끌고오기)
		//         끌고오는 범위가 넓고 피해 범위가 좁음.
		//         끌고오는 범위 내의 적들을 피해 범위로 끌어들인 다음 폭발
		// idea 4. 공처럼 통통 튀기면서 튀길 때마다 폭발, 3번까지, 직선형
		// idea 5. 산탄?
		// idea 6. 격발 후 키를 한번 더 누르면 그 위치(허공)에서 폭발, 그 아래 땅에
		//         범위형으로 데미지
		// 
		// 
		// extra idea 1. 바람에 펄럭이는 깃발?? 폭발에도 영향받기?? 내 주제에 이런게 되나
		// extra idea 2. 폭발하면 근방의 애들 밀려나기 (폭발 지점으로부터 반대방향)
		//
		//
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

	if (nullptr != m_pOpenGLDevice)
	{
		result = m_pOpenGLDevice->CreateOpenGLWindow(800, 600, "OpenGL Window");
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