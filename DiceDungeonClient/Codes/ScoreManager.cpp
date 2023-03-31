#include "ScoreManager.h"
#include "iDAO.h"
#include "DAO.h"
#include "LeaderboardManager.h"

#include <sstream>


SINGLETON_FUNCTION(ScoreManager)
USING(Engine)
USING(std)

_bool compare(ranking_info& a, ranking_info& b)
{
	return a.highScore > b.highScore;
}

ScoreManager::ScoreManager()
	: m_pDAO(nullptr)
{
}

ScoreManager::~ScoreManager()
{
}

// Call instead of destructor to manage class internal data
void ScoreManager::Destroy()
{
	SafeDestroy(m_pDAO);
}

// Initialize
RESULT ScoreManager::Ready(string filePath)
{
	stringstream ss;
	ss << filePath << "score.db";
	m_pDAO = CDAO::Create(ss.str());

	return PK_NOERROR;
}

// Set high score to the db file
string ScoreManager::AddNewScore(_int playerId, _int score)
{
	if (nullptr == m_pDAO)
		return "";

	string result("");
	_bool isHighScore = m_pDAO->setHighScore(playerId, score);
	if (isHighScore)
	{
		SendToServer(playerId, score);
		result = "Congratulation!! New high score!";
	}
	else
	{
		result = "The score has not been updated...";
	}

	return result;
}

// Get high score from the db file
_int ScoreManager::GetHighScore(_int playerId)
{
	if (nullptr == m_pDAO)
		return 0;

	_int score = m_pDAO->getHighScore(playerId);

	return score;
}

void ScoreManager::GetTop20()
{
	m_vecRanking20.clear();
	map<_int, ranking_info>* pmap = LeaderboardManager::GetInstance()->GetTop20();

	if (nullptr == pmap)
		return;

	map<_int, ranking_info>::iterator iter;
	for (iter = pmap->begin(); iter != pmap->end(); ++iter)
		m_vecRanking20.push_back(iter->second);

	sort(m_vecRanking20.begin(), m_vecRanking20.end(), compare);
}

// Send the score to the leaderboard server
void ScoreManager::SendToServer(_int playerId, _int score)
{
	LeaderboardManager::GetInstance()->SetHighScore(playerId, score);
}
