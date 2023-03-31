#include "UIManager.h"
#include "..\imgui\imgui_impl_glfw.h"
#include "..\imgui\imgui_impl_opengl3.h"
#include "OpenGLDevice.h"
#include "Define.h"
#include "DiceMaster.h"
#include "iPhysicsWorld.h"
#include "ScoreManager.h"
#include "leaderboard_types.h"
#include "LeaderboardManager.h"

#include <sstream>
#include <iomanip>

SINGLETON_FUNCTION(UIManager)
USING(Engine)
USING(ImGui)
USING(std)

UIManager::UIManager()
	: m_pPWorld(nullptr), m_Message(""), m_iHighScore(0), m_bOpenRankingListPage(false)
{
	ZeroMemory(m_char, sizeof(m_char));
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

	ImVec2 screen = ImVec2(200.f, 100.f);
	SetNextWindowPos(ImVec2(0.f, 0.f));
	SetNextWindowSize(screen);
	if (Begin("DiceUI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar))
	{
		Text("Dice UI");
		Separator();

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

	screen.x = 300.f;
	screen.y = 220.f;
	SetNextWindowPos(ImVec2(width - screen.x, 0.f));
	SetNextWindowSize(screen);
	if (Begin("ScoreUI", (bool*)0,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar))
	{
		Text("ScoreUI");
		Separator();

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
		Separator();

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
		Separator();

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
		Separator();

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

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());
}

// Initialize
RESULT UIManager::Ready(iPhysicsWorld* pWorld)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();

	if (!ImGui_ImplGlfw_InitForOpenGL(COpenGLDevice::GetInstance()->GetWindow(), true) ||
		!ImGui_ImplOpenGL3_Init("#version 460"))
		return PK_ERROR_IMGUI;
	StyleColorsDark();

	m_pPWorld = pWorld;

	return PK_NOERROR;
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
