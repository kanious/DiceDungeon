#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_

#include "Base.h"

class Enemy;
class SceneDungeon;
class DefaultCamera;

// A class that searches for and manages the target object to be moved by the user
class EnemyManager : public Engine::CBase
{
	SINGLETON(EnemyManager)

private:
	SceneDungeon*				m_pScene;
	std::vector<Enemy*>			m_vecEnemies;
	_bool						m_bStartEnemyTurn;
	DefaultCamera*				m_pCamera;

	_int						m_iCurIdx;
	_bool						m_bFinishCurEnemy;

private:
	explicit EnemyManager();
	virtual ~EnemyManager();
public:
	// Call instead of destructor to manage class internal data
	void Destroy();
	// Initialize
	RESULT Ready(SceneDungeon* pScene);
	// Basic Update Function, check user input
	void Update(const _float& dt);

public:
	_bool GetEnemyTurn()			{ return m_bStartEnemyTurn; }
	void AddEnemy(Enemy* pEnemy);
	void StartEnemyTurn();
	void StopEnemyTurn();
	void SetFinishMyTurn();
};

#endif //_ENEMYMANAGER_H_