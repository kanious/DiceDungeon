#ifndef _SCOREMANAGER_H_
#define _SCOREMANAGER_H_

#include "Base.h"

// Class that manages scores using DAO and Sqlite DB files
class ScoreManager : public Engine::CBase
{
	SINGLETON(ScoreManager)

private:
	explicit ScoreManager();
	~ScoreManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready();

private:
	// Set high score to the db file
	void SetHighScore(_int playerId, _int score);
	// Get high score from the db file
	_int GetHighScore(_int playerId);
	// Send the score to the leaderboard server
	void SendToServer(_int playerId, _int score);
};

#endif //_SCOREMANAGER_H_