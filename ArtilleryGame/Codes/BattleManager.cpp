#include "BattleManager.h"
#include "GameMaster.h"
#include "ConsoleManager.h"
#include "Enemy.h"
#include "BoomEffect.h"
#include "GameObject.h"
#include "Scene.h"
#include "Layer.h"
#include <math.h>


SINGLETON_FUNCTION(BattleManager)
USING(Engine)

BattleManager::BattleManager()
{
	m_iBulletCount = 0;
	m_pEnemy = nullptr;
	m_bIsFiring = false;
	m_bGameEnd = false;
}

BattleManager::~BattleManager()
{
}

void BattleManager::Destroy()
{
	DestroyInstance();
}

void BattleManager::AddBulletNum()
{
	++m_iBulletCount;
	++m_iAllBulletCount;
}

void BattleManager::SubtractBulletNum()
{
	--m_iBulletCount;

	if (m_iBulletCount == 0)
	{
		if (m_pEnemy->GetCurrentHP() <= 0)
		{
			m_bGameEnd = true;

			if (!m_bESCPressed)
				ConsoleManager::GetInstance()->AddWinText(m_iAllBulletCount);
		}

		ConsoleManager::GetInstance()->WriteText();
		m_bIsFiring = false;
	}
}

void BattleManager::BoomLocation(vec3 vPos, _float radius, _float power)
{
	if (nullptr == m_pEnemy)
		return;

	vec3 vEnemyPos = m_pEnemy->GetPosition();
	_float fDistance = sqrt(pow(vPos.x - vEnemyPos.x, 2) + pow(vPos.y - vEnemyPos.y, 2)
		+ pow(vPos.z - vEnemyPos.z, 2));

	MakeBoomEffect(vPos, radius);

	// Hit
	if (fDistance <= radius)
	{
		vec3 vDir = vEnemyPos - vPos;
		vDir = normalize(vDir);
		m_pEnemy->Hit(vDir, power);
		m_pEnemy->SetDamage((_int)power);

		_int hp = m_pEnemy->GetCurrentHP();
		ConsoleManager::GetInstance()->AddEnemyHitText(hp, m_pEnemy->GetMaxHP(), fDistance);
	}
	// Miss
	else
	{
		ConsoleManager::GetInstance()->AddEnemyMissText(m_pEnemy->GetCurrentHP()
			, m_pEnemy->GetMaxHP(), fDistance);
	}
}

void BattleManager::MakeBoomEffect(vec3 vPos, _float radius)
{
	CScene* pScene = CGameMaster::GetInstance()->GetCurrentScene();
	if (nullptr == pScene)
		return;
	CLayer* pLayer = pScene->GetLayer((_uint)LAYER_OBJECT);
	if (nullptr == pLayer)
		return;

	vec3 vRot(0.f);
	vec3 vScale(0.f);

	CGameObject* pGameObject = BoomEffect::Create((eSCENETAG)pScene->GetSceneTag(),
		LAYER_OBJECT, OBJ_PROJECTILE, vPos, vRot, vScale, radius);
	if (nullptr != pGameObject)
	{
		pLayer->AddGameObject(pGameObject);
		pGameObject->SetParentLayer(pLayer);
	}
}

void BattleManager::SetNewGame()
{
	m_iBulletCount = 0;
	m_pEnemy = nullptr;
	m_bIsFiring = false;
	m_bGameEnd = false;
}
