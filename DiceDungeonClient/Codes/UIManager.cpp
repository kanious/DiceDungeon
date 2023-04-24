#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "OpenGLDefines.h"
#include "Define.h"
#include "DiceMaster.h"
#include "iPhysicsWorld.h"
#include "ScoreManager.h"
#include "Player.h"
#include "leaderboard_types.h"
#include "LeaderboardManager.h"
#include "SceneDungeon.h"
#include "UIHealthInfo.h"
#include "Renderer.h"
#include "LuaBrain.h"
#include "SoundMaster.h"

#include <sstream>
#include <iomanip>

SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)
USING(glm)

UIManager::UIManager()
	: m_pPWorld(nullptr), m_Message(""), m_iHighScore(0), m_bOpenRankingListPage(false)
	, m_bOpenPlayerMoveW(false), m_vecMoveTilePos(vec3(0.f)), m_bDeferredTexture(false)
	, m_bHPUI(true), m_bBGPlay(true), m_bBGPlayPrev(false), m_bAnimBlending(true)
	, m_bNoise(true), m_fNoiseAmount(0.07f)
{
	ZeroMemory(m_char, sizeof(m_char));
	m_vecHPInfos.clear();
}

UIManager::~UIManager()
{
}

// Call instead of destructor to manage class internal data
void UIManager::Destroy()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

// Basic Render Function
void UIManager::RenderUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	NewFrame();

	_float width = (_float)COpenGLDevice::GetInstance()->GetWidthSize();
	_float height = (_float)COpenGLDevice::GetInstance()->GetHeightSize();

	RenderDiceWindow();
	RenderScoreWindow(width, height);
	RenderDebugUI(width, height);

	if (m_bOpenPlayerMoveW)
		RenderPlayerMoveButtonWindow(width, height);

	if (m_bHPUI)
	{
		for (int i = 0; i < m_vecHPInfos.size(); ++i)
			m_vecHPInfos[i]->RenderUI();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

// Initialize
RESULT UIManager::Ready(iPhysicsWorld* pWorld, SceneDungeon* pScene)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

	m_pPWorld = pWorld;
	m_pScene = pScene;
	if (nullptr != pScene)
		m_pPlayer = pScene->GetPlayer();

	return PK_NOERROR;
}

void UIManager::RenderDiceWindow()
{
	ImVec2 screen = ImVec2(200.f, 100.f);
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(screen);
	if (Begin("DiceUI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse))
	{
		if (Button("Roll2", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(2);
		}
		SameLine(65.f);
		if (Button("Roll3", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(3);
		}
		SameLine(120.f);
		if (Button("Roll5", ImVec2(50.f, 0.f)))
		{
			if (nullptr != m_pPWorld)
				m_pPWorld->RollDice(5);
		}

		Text("Rolled Dice:"); SameLine(115.f);
		stringstream ss;
		ss << CDiceMaster::GetInstance()->GetRolledDiceCount();
		Text(ss.str().c_str());

		Text("AP:"); SameLine(115.f);
		ss.str("");
		ss << CDiceMaster::GetInstance()->GetAP();
		Text(ss.str().c_str());

		End();
	}
}

void UIManager::RenderScoreWindow(_float width, _float height)
{
	ImVec2 screen = ImVec2(300.f, 220.f);
	SetNextWindowPos(ImVec2(width - screen.x, 0.f));
	SetNextWindowSize(screen);
	if (Begin("ScoreUI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar))
	{
		Text("ScoreUI");
		ImGui::Separator();

		Text("Server status: "); SameLine(150.f);
		_bool connected = LeaderboardManager::GetInstance()->GetConnected();
		if (connected)
			Text("Connected");
		else
			Text("DISCONNECTED");
		if (Button("Connect", ImVec2(140.f, 0.f)))
		{
			if (!connected)
				LeaderboardManager::GetInstance()->ConnectWithServer();
		}
		SameLine(150.f);
		if (Button("Disconnect", ImVec2(140.f, 0.f)))
		{
			if (connected)
				LeaderboardManager::GetInstance()->DisconnectWithServer();
		}
		ImGui::Separator();

		Text("* Add new score");
		Text("Id:"); SameLine(30.f);
		SetNextItemWidth(35); InputText("##NewId", m_char[0], sizeof(m_char[0]));
		SameLine(75.f); Text("Score:"); SameLine(120.f);
		SetNextItemWidth(55); InputText("##NewScore", m_char[1], sizeof(m_char[1]));
		SameLine(190.f);
		if (Button("Add new score", ImVec2(100.f, 0.f)))
		{
			_int playerId = atoi(m_char[0]);
			_int score = atoi(m_char[1]);
			m_Message = ScoreManager::GetInstance()->AddNewScore(playerId, score);
		}
		Text(m_Message.c_str());
		ImGui::Separator();

		Text("* Get the score by playerId");
		Text("Id:"); SameLine(30.f);
		SetNextItemWidth(35); InputText("##GetId", m_char[2], sizeof(m_char[2]));
		SameLine(75.f); Text("Score:"); SameLine(120.f);
		SetNextItemWidth(55); stringstream ss; ss << m_iHighScore; Text(ss.str().c_str());
		SameLine(190.f);
		if (Button("Get highscore", ImVec2(100.f, 0.f)))
		{
			_int playerId = atoi(m_char[2]);
			m_iHighScore = ScoreManager::GetInstance()->GetHighScore(playerId);
		}
		ImGui::Separator();

		Text("* Get Top 20 list"); SameLine(190.f);
		if (Button("Top 20", ImVec2(100.f, 0.f)))
		{
			ScoreManager::GetInstance()->GetTop20();
			m_bOpenRankingListPage = true;
		}

		if (m_bOpenRankingListPage)
			RenderRankingPage(width, height);

		End();
	}
}

void UIManager::RenderDebugUI(_float width, _float height)
{
	ImVec2 screen = ImVec2(200.f, 500.f);
	SetNextWindowPos(ImVec2(0.f, 100.f));
	SetNextWindowSize(screen);
	if (Begin("Debug", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse))
	{
		Text("Game Setting");
		if (Button("Reset All", ImVec2(180.f, 0.f)))
		{
			if (nullptr != m_pScene)
				m_pScene->GameReset();
		}
		if (Button("Enemy Turn Start", ImVec2(180.f, 0.f)))
		{
			if (nullptr != m_pScene)
				m_pScene->StartEnemyTurn();
		}

		if (Button("Enemy Turn Stop", ImVec2(180.f, 0.f)))
		{
			if (nullptr != m_pScene)
				m_pScene->StopEnemyTurn();
		}

		Separator();
		Text("Player Animation");

		if (Button("idle", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_idle");
		}
		SameLine(100.f);
		if (Button("walk", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_walk");
		}

		if (Button("run", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_run");
		}
		SameLine(100.f);
		if (Button("attack1", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_attack1");
		}

		if (Button("attack2", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_attack2");
		}
		SameLine(100.f);
		if (Button("grab", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_grab");
		}

		if (Button("pick up", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_pick_up");
		}
		SameLine(100.f);
		if (Button("hurt", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_hurt");
		}

		if (Button("death", ImVec2(90.f, 0.f)))
		{
			if (nullptr != m_pPlayer)
				m_pPlayer->ChangeAnimation("true_pumpkin_king_death");
		}
		Checkbox("Animation Blending", &m_bAnimBlending);
		m_pPlayer->SetAnimationBlending(m_bAnimBlending);

		Separator();
		Text("Toggle");
		Checkbox("Show Health UI", &m_bHPUI);
		Checkbox("Play BG", &m_bBGPlay);
		if (m_bBGPlay != m_bBGPlayPrev)
		{
			m_bBGPlayPrev = m_bBGPlay;
			if (m_bBGPlay)
				CLuaBrain::GetInstance()->RunLuaScript("BG");
			else
				CSoundMaster::GetInstance()->StopSound("Background");
		}

		Separator();
		Text("Graphics");
		Checkbox("Show Deferred Texture", &m_bDeferredTexture);
		CRenderer::GetInstance()->SetShowDebug(m_bDeferredTexture);

		Checkbox("Show Noise Option", &m_bNoise);
		CRenderer::GetInstance()->SetNoiseOption(m_bNoise);

		stringstream ss;
		ss << m_fNoiseAmount;
		strcpy_s(m_char[3], ss.str().length() + 1, ss.str().c_str());
		Text("Noise Amount :"); SameLine(110.f);
		SetNextItemWidth(35); InputText("##NoiseAmountInput", m_char[3], sizeof(m_char[3]));
		m_fNoiseAmount = atof(m_char[3]);
		CRenderer::GetInstance()->SetNoiseAmount(m_fNoiseAmount);

		End();
	}
}

void UIManager::SetPlayerMoveButtonWindow(_bool bOpen, vec3 vPos)
{
	m_bOpenPlayerMoveW = bOpen;
	m_vecMoveTilePos = vPos;
}

void UIManager::AddHealthUI(UIHealthInfo* pUI)
{
	m_vecHPInfos.push_back(pUI);
}

_bool UIManager::GetCursorIsOnTheUI()
{
	return GetIO().WantCaptureMouse;
}

void UIManager::RenderRankingPage(_float width, _float height)
{
	ImVec2 windowSize = ImVec2(200.f, 415.f);
	SetNextWindowPos(ImVec2(width / 2.f - (windowSize.x / 2.f), height / 2.f - (windowSize.y / 2.f)));
	SetNextWindowSize(windowSize);
	if (Begin("Ranking List", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove))
	{
		stringstream ss;
		vector<ranking_info>* pvecInfo = ScoreManager::GetInstance()->GetRanking20();
		for (int i = 0; i < pvecInfo->size(); ++i)
		{
			ranking_info info = (*pvecInfo)[i];

			ss.str("");
			ss << "[" << i + 1 << "] " << info.playerId << ":" << info.name;
			Text(ss.str().c_str());
			SameLine(145.f);
			ss.str("");
			ss << info.highScore;
			Text(ss.str().c_str());
		}

		if (20 > pvecInfo->size())
		{
			for (int i = pvecInfo->size(); i < 20; ++i)
			{
				ss.str("");
				ss << "[" << i + 1 << "]     -           -";
				Text(ss.str().c_str());
			}
		}

		Text(" ");
		Separator();
		if (Button("Close", ImVec2(180.f, 0.f)))
		{
			m_bOpenRankingListPage = false;
		}

		End();
	}
}

void UIManager::RenderPlayerMoveButtonWindow(_float width, _float height)
{
	mat4x4 matWorld, matView, matProj;
	matWorld = mat4x4(1.f);
	matWorld = translate(matWorld, m_vecMoveTilePos);
	matView = COpenGLDevice::GetInstance()->GetViewMatrix();
	matProj = COpenGLDevice::GetInstance()->GetProjMatrix();
	vec4 screenPos = vec4(0.f, 0.f, 0.f, 1.f);
	screenPos = matProj * matView * matWorld * screenPos;
	_float screenX = screenPos.x / screenPos.w;
	_float screenY = screenPos.y / screenPos.w;
	screenX = (screenX + 1) * 0.5f * width;
	screenY = (screenY + 1) * 0.5f;
	screenY = 1 - screenY;
	screenY = (screenY * height) - 100.f;

	ImVec2 windowSize = ImVec2(200.f, 35.f);
	SetNextWindowPos(ImVec2(screenX - (windowSize.x / 2.f), screenY - (windowSize.y / 2.f)));
	SetNextWindowSize(windowSize);
	if (Begin("Player Move Button Window", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove))
	{
		if (Button("Move", ImVec2(180.f, 0.f)))
		{
			if (nullptr != m_pScene)
			{
				m_bOpenPlayerMoveW = false;
				m_pScene->MovePlayer();
			}
		}

		End();
	}
}
