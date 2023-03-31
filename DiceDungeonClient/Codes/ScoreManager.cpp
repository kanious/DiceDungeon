#include "ScoreManager.h"


SINGLETON_FUNCTION(ScoreManager)
USING(Engine)
USING(std)

ScoreManager::ScoreManager()
{
}

ScoreManager::~ScoreManager()
{
}

// Call instead of destructor to manage class internal data
void ScoreManager::Destroy()
{
}

// Initialize
RESULT ScoreManager::Ready()
{
	return PK_NOERROR;
}

// Set high score to the db file
void ScoreManager::SetHighScore(_int playerId, _int score)
{
}

// Get high score from the db file
_int ScoreManager::GetHighScore(_int playerId)
{
	return _int();
}

// Send the score to the leaderboard server
void ScoreManager::SendToServer(_int playerId, _int score)
{
}
