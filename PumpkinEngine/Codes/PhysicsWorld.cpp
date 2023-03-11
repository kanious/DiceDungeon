#include "pch.h"
#include "../Headers/PhysicsWorld.h"
#include "../Headers/iRigidBody.h"
#include "../Headers/RigidBody.h"
#include "../Headers/CollisionHandler.h"
#include "../Headers/iShape.h"
#include "../Headers/CollisionBox.h"
#include "../Headers/CollisionDetector.h"
#include "../Headers/RigidBodyDesc.h"
#include "../Headers/Component.h"
#include "../Headers/Camera.h"
#include "../Headers/DiceMaster.h"
#include "../Headers/EngineFunction.h"

USING(Engine)
USING(std)
USING(glm)

CPhysicsWorld::CPhysicsWorld()
	: m_vGravity(vec3(0.f)), m_collisionCallback(nullptr), m_iMaxContacts(0)
	, m_fElapsedTime(0.f), m_fMaxTime(1.5f), m_bRolling(false)
{
	m_vecRigidBodies.clear();
}

CPhysicsWorld::~CPhysicsWorld()
{
}

// Call instead of destructor to manage class internal data
void CPhysicsWorld::Destroy()
{
	for (int i = 0; i < m_vecRigidBodies.size(); ++i)
		SafeDestroy(m_vecRigidBodies[i]);

	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
		delete CCollisionBox::boxData[i];

	SafeDestroy(m_pColHandler);
}

// Basic Update Function, update all rigid bodies
void CPhysicsWorld::Update(const _float& dt)
{
	_bool allFinished = true;
	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		CCollisionBox* box = CCollisionBox::boxData[i];
		if (!box->body->isFinished)
		{
			allFinished = false;
			m_fElapsedTime = 0.f;
			break;
		}
	}
	if (allFinished)
	{
		m_fElapsedTime += dt;
		if (m_fElapsedTime > m_fMaxTime)
		{
			for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
				CCollisionBox::boxData[i]->body->SetEnable(false);
			m_bRolling = false;
			m_fElapsedTime = 0.f;
		}
	}

	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		CCollisionBox* box = CCollisionBox::boxData[i];

		box->body->SetGravity(m_vGravity);
		box->body->Update(dt);
		box->body->Integrate(dt);
		box->body->KillForces();
		box->CalculateInternals();
	}

	m_cData.Reset(m_iMaxContacts);
	m_cData.friction = 0.9f;
	m_cData.restitution = 0.1f;
	m_cData.tolerance = 0.1f;
	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		for (unsigned int j = 0; j < CCollisionBox::boxCount; ++j)
		{
			CCollisionBox* box1 = CCollisionBox::boxData[i];
			CCollisionBox* box2 = CCollisionBox::boxData[j];

			if (box1 != box2)
			{
				CCollisionDetector::BoxAndBox(*box1, *box2, &m_cData);
			}
		}
	}

	m_cResolver.ResolveContacts(m_cData.contactArray, m_cData.contactCount, dt);

	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		CCollisionBox* box = CCollisionBox::boxData[i];
		box->SetModelMatrix(scale(
			(mat4)box->body->transformMatrix,
			vec3(box->halfSize.x, box->halfSize.y, box->halfSize.z)
		));
	}
}

// Set gravity
void CPhysicsWorld::SetGravity(const vec3& gravity)
{
	m_vGravity = gravity;
}

// Add rigid body and initailize collision box
void CPhysicsWorld::AddBody(iRigidBody* body)
{
	if (nullptr == body)
		return;

	CRigidBody* rigidBody = dynamic_cast<CRigidBody*>(body);
	CCollisionBox* box = new CCollisionBox(rigidBody);
	box->body->isFinished = true;
	box->body->SetEnable(false);

	for (int i = 0; i < m_vecRigidBodies.size(); ++i)
	{
		if (rigidBody == m_vecRigidBodies[i])
			return;
	}
	m_vecRigidBodies.push_back(rigidBody);
}

// Remove rigid body
void CPhysicsWorld::RemoveBody(iRigidBody* body)
{
	//CRigidBody* rigidBody = dynamic_cast<CRigidBody*>(body);

	//vector<CRigidBody*>::iterator iter;
	//for (iter = m_vecRigidBodies.begin(); iter != m_vecRigidBodies.end(); ++iter)
	//{
	//	if (rigidBody == (*iter))
	//	{
	//		SafeDestroy(*iter);
	//		m_vecRigidBodies.erase(iter);
	//		return;
	//	}
	//}
}

// Roll dice
void CPhysicsWorld::RollDice(_uint count)
{
	//if (m_bRolling)
	//	return;

	m_bRolling = true;

	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		CCollisionBox* box = CCollisionBox::boxData[i];
		box->body->SetEnable(false);
	}

	if (count > 10)
		count = 10;
	if (count > CCollisionBox::boxCount - 1)
		count = CCollisionBox::boxCount - 1;

	CDiceMaster::GetInstance()->SetRolledDiceCount(count);
	CDiceMaster::GetInstance()->SetAP(0);

	vec3 vEyePos = m_pCamera->GetCameraEye();
	vec3 vTargetPos = m_pCamera->GetCameraTarget();
	vec3 vDir = vTargetPos - vEyePos;
	vDir.y = 0.f;
	vDir = normalize(vDir);
	vEyePos += vDir * 10.f;
	vEyePos.y = 15.f;

	CRigidBodyDesc desc;
	for (unsigned int i = 0; i < CCollisionBox::boxCount; ++i)
	{
		if (0 >= count)
			break;
		CCollisionBox* box = CCollisionBox::boxData[i];

		if (eShapeType::Plane == box->body->eShape)
			continue;

		vec3 vPos = vEyePos;
		_int randNum = GetRandNum(-200, 200);
		vPos.x += randNum * 0.01f * count;
		randNum = GetRandNum(-200, 200);
		vPos.z += randNum * 0.01f * count;

		_float force = (_float)GetRandNum(5000.f, 6500.f);

		_float randRotX = GetRandNum(-200, 200);
		_float randRotY = GetRandNum(-200, 200);
		_float randRotZ = GetRandNum(-200, 200);

		desc.position = vPos;
		desc.rotation = vec3(randRotX * 0.01f, randRotY * 0.01f, randRotZ * 0.01f);
		desc.halfSize = vec3(1.f);
		desc.orientation = vec3(0.f);
		desc.forceAccum = vDir * force;
		
		box->body->ResetRigidBody(desc);
		box->SetModelMatrix(scale(
			(mat4)box->body->transformMatrix,
			vec3(box->halfSize.x, box->halfSize.y, box->halfSize.z)
		));

		--count;
	}
}

// Set camera object
void CPhysicsWorld::SetCamera(CComponent* pCamera)
{
	if (nullptr != pCamera)
		m_pCamera = dynamic_cast<CCamera*>(pCamera);
}

// Initialize
RESULT CPhysicsWorld::Ready(function<void(void)> callback)
{
	m_pColHandler = CCollisionHandler::Create();

	m_collisionCallback = callback;

	m_iMaxContacts = 256;
	m_cResolver = CContactResolver(m_iMaxContacts);
	m_cData.contactArray = m_contacts;

	return PK_NOERROR;
}

// Create an instance
CPhysicsWorld* CPhysicsWorld::Create(function<void(void)> callback)
{
	CPhysicsWorld* pInstance = new CPhysicsWorld();
	if (PK_NOERROR != pInstance->Ready(callback))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
