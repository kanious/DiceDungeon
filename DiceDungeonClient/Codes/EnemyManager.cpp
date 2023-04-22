#include "EnemyManager.h"
#include "Enemy.h"
#include "SceneDungeon.h"
#include "DefaultCamera.h"

SINGLETON_FUNCTION(EnemyManager)
USING(Engine)
USING(std)
USING(glm)

EnemyManager::EnemyManager()
	: m_pScene(nullptr), m_bStartEnemyTurn(false), m_iCurIdx(0), m_bFinishCurEnemy(true)
{
	m_vecEnemies.clear();
}

EnemyManager::~EnemyManager()
{
}

// Call instead of destructor to manage class internal data
void EnemyManager::Destroy()
{
	m_vecEnemies.clear();
}

// Initialize
RESULT EnemyManager::Ready(SceneDungeon* pScene)
{
	m_pScene = pScene;
	m_pCamera = m_pScene->GetCamera();

	return PK_NOERROR;
}

// Basic Update Function, check user input
void EnemyManager::Update(const _float& dt)
{
	if (m_bStartEnemyTurn)
	{
		Enemy* pCurEnemy = m_vecEnemies[m_iCurIdx];
		m_pCamera->SetCameraTarget(pCurEnemy->GetPosition());

		if (m_bFinishCurEnemy)
		{
			++m_iCurIdx;
			if (m_iCurIdx >= m_vecEnemies.size())
			{
				StopEnemyTurn();
				return;
			}
			m_bFinishCurEnemy = false;
			pCurEnemy = m_vecEnemies[m_iCurIdx];
			pCurEnemy->RunAI();
		}
	}
}

void EnemyManager::AddEnemy(Enemy* pEnemy)
{
	m_vecEnemies.push_back(pEnemy);
}

void EnemyManager::StartEnemyTurn()
{
	if (m_bStartEnemyTurn)
		return;

	m_bStartEnemyTurn = true;
	m_pCamera->SetFixed(true);
	m_bFinishCurEnemy = false;

	m_iCurIdx = 0;
	m_vecEnemies[0]->RunAI();
}

void EnemyManager::StopEnemyTurn()
{
	for (int i = 0; i < m_vecEnemies.size(); ++i)
		m_vecEnemies[i]->EndTurn();

	m_bStartEnemyTurn = false;
	m_pCamera->SetFixed(false);
	vec3 vPlayerPos(0.f);
	if (m_pScene->GetPlayerPos(vPlayerPos))
	{
		m_pCamera->SetCameraTarget(vPlayerPos);
		m_pCamera->ResetCameraPos();
	}
	m_iCurIdx = 0;
	m_bFinishCurEnemy = false;
}

void EnemyManager::SetFinishMyTurn()
{
	m_bFinishCurEnemy = true;
}
