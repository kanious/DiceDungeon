#include "PhysicsSystem.h"
#include "InputDevice.h"
#include "SceneFPS.h"
#include "BGObject.h"
#include "Target.h"
#include "DefaultCamera.h"
#include "CollisionMaster.h"
#include "Define.h"
#include "Function.h"


SINGLETON_FUNCTION(PhysicsSystem)
USING(Engine)
USING(glm)
USING(std)

PhysicsSystem::PhysicsSystem()
	: m_pScene(nullptr), m_pDefaultCamera(nullptr), m_pHoverTarget(nullptr)
{
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
	m_vecBGs.clear();
	m_vecTargets.clear();
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::CollisionCheck()
{
	if (nullptr == m_pDefaultCamera || nullptr == m_pInputDevice)
		return;

	vec3 vCameraPos = m_pDefaultCamera->GetCameraEye();
	vec3 vDir = m_pInputDevice->GetCenterMouseWorldCoord();

	_float distanceMin = FLT_MAX;
	_int index = 0;
	_bool isTarget = false;
	m_pHoverTarget = nullptr;

	for (int i = 0; i < m_vecBGs.size(); ++i)
	{
		if (CCollisionMaster::GetInstance()->IntersectRayToBoundingBox(
			m_vecBGs[i]->GetBoundingBox(),
			m_vecBGs[i]->GetTransform(), vCameraPos, vDir))
		{
			if (CheckDir(vCameraPos, m_vecBGs[i]->GetPosition()))
			{
				_float dis = distance(vCameraPos, m_vecBGs[i]->GetPosition());
				if (dis < distanceMin)
				{
					distanceMin = dis;
					isTarget = false;
				}
			}
		}
	}

	for (int i = 0; i < m_vecTargets.size(); ++i)
	{
		m_vecTargets[i]->Hovered(false);

		if (CCollisionMaster::GetInstance()->IntersectRayToSphere(
			vCameraPos, vDir, m_vecTargets[i]->GetPosition(), SPHERE_RADIUS))
		{
			if (CheckDir(vCameraPos, m_vecTargets[i]->GetPosition()))
			{
				_float dis = distance(vCameraPos, m_vecTargets[i]->GetPosition());
				if (dis < distanceMin)
				{
					distanceMin = dis;
					index = i;
					isTarget = true;
				}
			}
		}
	}

	if (isTarget)
	{
		if (SHOT_MODE == m_eGameMode)
			m_vecTargets[index]->Boom();
		else
		{
			m_vecTargets[index]->Hovered(true);
			m_pHoverTarget = m_vecTargets[index];
		}
	}
	else
	{
		if (SHOT_MODE == m_eGameMode)
			++m_iMissCount;
	}
}

void PhysicsSystem::GetRandomMovingInfo(_float& fTime, _float& fAccel, glm::vec3& vTargetPos)
{
	_float newValue = GetRandNum(300, 800) / 100.f;
	fTime = newValue;

	newValue = GetRandNum(200, 500) / 10000.f;
	fAccel = newValue;

	newValue = GetRandNum(-4900, 4900) / 100.f;
	_float newValue2 = GetRandNum(-4900, 4900) / 100.f;
	vTargetPos = vec3(newValue, SPHERE_HEIGHT, newValue2);
}

_bool PhysicsSystem::CheckDir(glm::vec3 vCameraPos, glm::vec3 vTargetPos)
{
	vec3 vCameraLook = m_pDefaultCamera->GetCameraTarget() - vCameraPos;
	vCameraLook = normalize(vCameraLook);
	vec3 vTargetLook = vTargetPos - vCameraPos;
	vTargetLook = normalize(vTargetLook);
	
	_float value = dot(vCameraLook, vTargetLook);
	if (value < 0)
		return false;

	return true;
}

void PhysicsSystem::Destroy()
{
	SafeDestroy(m_pInputDevice);

	m_vecBGs.clear();
	m_vecTargets.clear();
}

void PhysicsSystem::Update(const _float& dt)
{
	if (SHOT_MODE == m_eGameMode)
	{
		static _bool isMouseClicked = false;
		if (m_pInputDevice->IsMousePressed(GLFW_MOUSE_BUTTON_1))
		{
			if (!isMouseClicked)
			{
				isMouseClicked = true;

				CollisionCheck();

			}
		}
		else
			isMouseClicked = false;
	}
	else
	{
		CollisionCheck();
	}
}

RESULT PhysicsSystem::Ready(SceneFPS* pScene, DefaultCamera* pCamera)
{
	m_pScene = pScene;
	m_pDefaultCamera = pCamera;

	return PK_NOERROR;
}
