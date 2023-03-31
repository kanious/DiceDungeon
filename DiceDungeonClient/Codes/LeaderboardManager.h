#ifndef _LEADERBOARDMANAGER_H_
#define _LEADERBOARDMANAGER_H_

#include "Base.h"
#include <map>

// Class that communicates with Leaderboard server
class LeaderboardManager : public Engine::CBase
{
	SINGLETON(LeaderboardManager)

private:

private:
	explicit LeaderboardManager();
	~LeaderboardManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready();

private:
	// Connect with Server
	void ConnectWithServer();
	// Set high score to the server
	void SetHighScore(_int playerId, _int score);
	// Get top 20 list from the server
	std::map<_int, _int> GetTop20();
};

#endif //_LEADERBOARDMANAGER_H_