#ifndef _SCOREMANAGER_H_
#define _SCOREMANAGER_H_

#include "Base.h"
#include "leaderboard_types.h"
#include <map>

namespace Engine
{
	class iDAO;
}

// Class that manages scores using DAO and Sqlite DB files
class ScoreManager : public Engine::CBase
{
	SINGLETON(ScoreManager)
private:
	Engine::iDAO*				m_pDAO;
	std::vector<ranking_info>	m_vecRanking20;

private:
	explicit ScoreManager();
	~ScoreManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(std::string filePath);

public:
	// Set high score to the db file
	std::string AddNewScore(_int playerId, _int score);
	// Get high score from the db file
	_int GetHighScore(_int playerId);
	// Get Top 20 list from the leaderboard server
	void GetTop20();
	std::vector<ranking_info>* GetRanking20() { return &m_vecRanking20; }

private:
	// Send the score to the leaderboard server
	void SendToServer(_int playerId, _int score);
};

#endif //_SCOREMANAGER_H_