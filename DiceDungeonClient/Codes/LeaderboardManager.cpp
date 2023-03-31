#include "LeaderboardManager.h"


SINGLETON_FUNCTION(LeaderboardManager)
USING(Engine)
USING(std)

LeaderboardManager::LeaderboardManager()
{
}

LeaderboardManager::~LeaderboardManager()
{
}

// Call instead of destructor to manage class internal data
void LeaderboardManager::Destroy()
{
}

// Initialize
RESULT LeaderboardManager::Ready()
{
	return PK_NOERROR;
}

// Connect with Server
void LeaderboardManager::ConnectWithServer()
{
}

// Set high score to the server
void LeaderboardManager::SetHighScore(_int playerId, _int score)
{
}

// Get top 20 list from the server
map<_int, _int> LeaderboardManager::GetTop20()
{
	return std::map<_int, _int>();
}
