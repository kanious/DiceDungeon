#include <sstream>

#include "Client.h"
#include "Define.h"
#include "GameMaster.h"
#include "Timer.h"
#include "OpenGLDevice.h"
#include "InputDevice.h"
#include "ComponentMaster.h"
#include "Camera.h"
#include "Transform.h"
#include "JsonParser.h"
#include "XMLParser.h"
#include <atlconv.h>

#include "Scene.h"
#include "SceneDungeon.h"
#include "UIManager.h"
#include "AnimationManager.h"
#include "TargetManager.h"
#include "ScoreManager.h"
#include "LeaderboardManager.h"
#include "EnemyManager.h"


USING(Engine)
USING(std)

Client::Client()
{
	m_iFPS = FPS;

	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, MAX_PATH);
	USES_CONVERSION;
	std::string str = W2A(path);
	str = str.substr(0, str.find_last_of("\\/"));
	stringstream ss;
	ss << str << "\\..\\Assets\\";
	m_DataPath = ss.str();

	m_SoundDataFileName = "sound.xml";
	m_ShaderDataFileName = "shader.xml";
	m_TextureDataFileName = "texture.json";
	m_MeshDataFileName = "mesh.json";
}

Client::~Client()
{
}

// Call instead of destructor to manage class internal data
void Client::Destroy()
{
	SafeDestroy(m_pTimer);
	SafeDestroy(m_pInputDevice);
	SafeDestroy(m_pGraphicDevice);
	SafeDestroy(m_pGameMaster);

	SafeDestroy(UIManager::GetInstance());
	SafeDestroy(AnimationManager::GetInstance());
	SafeDestroy(TargetManager::GetInstance());
	SafeDestroy(ScoreManager::GetInstance());
	SafeDestroy(LeaderboardManager::GetInstance());
	SafeDestroy(EnemyManager::GetInstance());

	delete this;
}

// Core Loop
void Client::Loop()
{
	_uint iFPS = 0;
	_float fCheckTime = 0.f;

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_MULTISAMPLE);

	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	while (true)
	{
		if (nullptr == m_pGameMaster || nullptr == m_pTimer)
			break;

		if (glfwWindowShouldClose(COpenGLDevice::GetInstance()->GetWindow()) ||
			m_pInputDevice->IsKeyDown(GLFW_KEY_ESCAPE))
			break;

		if (m_pGameMaster->GetGameClose())
			break;
	
		m_pTimer->Update();
		if (m_pTimer->IsUpdateAvailable())
		{
			//m_pGraphicDevice->GetWindowSize();
			//glViewport(0, 0, m_pGraphicDevice->GetWidthSize(), m_pGraphicDevice->GetHeightSize());
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			_float dt = m_pTimer->GetTimeDelta();
			m_pGameMaster->Update(dt);
			m_pGameMaster->Render();

			glfwSwapBuffers(m_pGraphicDevice->GetWindow());
			glfwPollEvents();

			iFPS++;
		}

		fCheckTime += m_pTimer->GetTimeDefault();
		if (fCheckTime >= 1.f)
		{
			stringstream ssTitle;
			ssTitle << "FPS: " << iFPS;
			glfwSetWindowTitle(m_pGraphicDevice->GetWindow(), ssTitle.str().c_str());

			iFPS = 0;
			fCheckTime = 0.f;
		}
	}
}

// Initialize
RESULT Client::Ready()
{
	RESULT result = PK_NOERROR;

	// OpenGL Graphic Device
	m_pGraphicDevice = COpenGLDevice::GetInstance(); m_pGraphicDevice->AddRefCnt();
	result = m_pGraphicDevice->CreateOpenGLWindow(1920, 1080, "OpenGL Window", false, false);
	if (PK_NOERROR != result)
		return result;

	// Load Components
	result = Ready_BasicComponent();
	if (PK_NOERROR != result)
		return result;

	// Timer
	m_pTimer = CTimer::GetInstance(); m_pTimer->AddRefCnt();
	m_pTimer->SetFrameRate(m_iFPS);

	// Input Device
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	result = m_pInputDevice->SetupInputSystem(m_pGraphicDevice->GetWindow(), GLFW_CURSOR_NORMAL);
	if (PK_NOERROR != result)
		return result;

	// Game Master
	m_pGameMaster = CGameMaster::GetInstance();
	m_pGameMaster->SetAssetPath(m_DataPath);
	m_pGameMaster->Ready();

	// Load Scene
	if (nullptr != m_pGameMaster)
	{
		CScene* pScene = SceneDungeon::Create(m_DataPath);
		m_pGameMaster->SetCurrentScene(pScene);
	}

	return PK_NOERROR;
}

// Initialize Basic Components
RESULT Client::Ready_BasicComponent()
{
	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Create.Camera
	pComponent = CCamera::Create();
	if (nullptr != pComponent)
		pMaster->AddNewComponent("Camera", pComponent);
	else
		return PK_CAMERA_CREATE_FAILED;

	//Create.Transform
	pComponent = CTransform::Create();
	if (nullptr != pComponent)
		pMaster->AddNewComponent("Transform", pComponent);
	else
		return PK_TRANSFORM_CREATE_FAILED;

	CXMLParser::GetInstance()->LoadSoundData(m_DataPath, m_SoundDataFileName);
	CXMLParser::GetInstance()->LoadShaderData(m_DataPath, m_ShaderDataFileName);

	CJsonParser::GetInstance()->SetAssetDataPath(m_DataPath);
	CJsonParser::GetInstance()->LoadTextureData(m_TextureDataFileName);
	CJsonParser::GetInstance()->LoadMeshData(m_MeshDataFileName, false);

	ScoreManager::GetInstance()->Ready(m_DataPath);
	LeaderboardManager::GetInstance()->Ready();

	return PK_NOERROR;
}
