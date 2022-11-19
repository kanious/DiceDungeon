#include "Character.h"
#include "ComponentMaster.h"
#include "Layer.h"

#include "Component.h"
#include "Transform.h"
#include "Mesh.h"
#include "Renderer.h"
#include "BoundingBox.h"
#include "OpenGLDefines.h"
#include "OpenGLDevice.h"
#include "SoundMaster.h"
#include "Octree.h"
#include "InputDevice.h"
#include "BGObject.h"
#include "CollisionMaster.h"


USING(Engine)
USING(glm)
USING(std)

Character::Character()
	: m_pMesh(nullptr), m_vVelocity(vec3(0.f)), m_vAccel(vec3(0.f)), m_vForce(vec3(0.f))
	, m_pTerrain(nullptr), m_pTerrainOctree(nullptr)
{
	m_bDebug = false;
	m_pInputDevice = CInputDevice::GetInstance(); m_pInputDevice->AddRefCnt();
}

Character::~Character()
{
}

void Character::SetTerrain(BGObject* terrain)
{
	if (nullptr == terrain)
		return;

	m_pTerrain = terrain;
	m_pTerrainOctree = m_pTerrain->GetOctreeFromMesh();
}

TRIANGLE* Character::GetTrianglesFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleArray();

	return nullptr;
}

_uint Character::GetTriangleNumber()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetTriangleNumber();

	return 0;
}

COctree* Character::GetOctreeFromMesh()
{
	if (nullptr != m_pMesh)
		return m_pMesh->GetOctree();

	return nullptr;
}

void Character::KeyCheck(const _float& dt)
{
	if (nullptr == m_pTransform || nullptr == m_pInputDevice)
		return;

	if (m_pInputDevice->IsKeyDown(GLFW_KEY_UP))
	{
		AddForce(vec3(0.f, 0.f, -1.f) * 3.f);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_DOWN))
	{
		AddForce(vec3(0.f, 0.f, 1.f) * 3.f);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_LEFT))
	{
		AddForce(vec3(-1.f, 0.f, 0.f) * 3.f);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_RIGHT))
	{
		AddForce(vec3(1.f, 0.f, 0.f) * 3.f);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_O))
	{
		AddForce(vec3(0.f, 1.f, 0.f) * 3.f);
	}
	if (m_pInputDevice->IsKeyDown(GLFW_KEY_L))
	{
		AddForce(vec3(0.f, -1.f, 0.f) * 3.f);
	}
}

void Character::CollisionCheck()
{
	if (nullptr == m_pTerrain || nullptr == m_pTerrainOctree)
		return;

	//m_pOctree->UpdateAABB(m_pTransform);
	m_pBoundingBox->UpdateAABB(m_pTransform);
	m_pMesh->SetDebugBox(false);

	vector<COctree::COctreeNode*> vecNode;
	m_pTerrainOctree->CheckBoundingBox(m_pBoundingBox, vecNode);
	for (int i = 0; i < vecNode.size(); ++i)
	{
		for (int j = 0; j < vecNode[i]->vecTriangles.size(); ++j)
		{
			TRIANGLE* tri = vecNode[i]->vecTriangles[j];
			//_int result = CCollisionMaster::GetInstance()->IntersectTriangleToOBB(tri, m_pBoundingBox);
			_bool result = CCollisionMaster::GetInstance()->IntersectTriangleToOBB(tri, m_pBoundingBox);
			if (result)
			{
				if (m_vForce.z < 0.f)
					m_vForce.z = 0.f;
				m_pMesh->SetDebugBox(result);
				return;
			}
		}
	}

	//for (int i = 0; i < vecNode.size(); ++i)
	//{
	//	for (int j = 0; j < vecNode[i]->vecTriangles.size(); ++j)
	//	{
	//		CAABB* tri = vecNode[i]->vecTriangles[j];
	//		_int result = m_pOctree->CheckAABBToTriangle(tri);

	//		switch (result)
	//		{
	//		case 1:
	//			if (m_vForce.x < 0.f)
	//				m_vForce.x = 0.f;
	//			break;
	//		case 2:
	//			if (m_vForce.x > 0.f)
	//				m_vForce.x = 0.f;
	//			break;
	//		case 3:
	//			if (m_vForce.y < 0.f)
	//				m_vForce.y = 0.f;
	//			break;
	//		case 4:
	//			if (m_vForce.y > 0.f)
	//				m_vForce.y = 0.f;
	//			break;
	//		case 5:
	//			if (m_vForce.z < 0.f)
	//				m_vForce.z = 0.f;
	//			break;
	//		case 6:
	//			if (m_vForce.z > 0.f)
	//				m_vForce.z = 0.f;
	//			break;
	//		default:
	//			break;
	//		}
	//	}
	//}
}

void Character::Update(const _float& dt)
{
	if (m_bEnable)
	{
		KeyCheck(dt);
		CollisionCheck();

		m_vAccel = m_vForce;
		m_vVelocity = (m_vAccel * dt);
		m_pTransform->AddPosition(m_vVelocity);
		m_vForce = vec3(0.f);

		CGameObject::Update(dt);

		if (nullptr != m_pRenderer)
			m_pRenderer->AddRenderObj(this);

		//if (nullptr != m_pMesh)
		//{
		//	m_pMesh->SetWireFrame(false);
		//	m_pMesh->SetDebugBox(false);
		//}
	}
}

void Character::Render()
{
	CGameObject::Render();
}

void Character::Destroy()
{
	SafeDestroy(m_pInputDevice);

	CGameObject::Destroy();
}

RESULT Character::Ready(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
{
	SetupGameObject(sTag, lTag, oTag);
	m_pLayer = pLayer;
	m_meshName = meshID;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	//Clone.Mesh
 	m_pMesh = CloneComponent<CMesh*>(meshID);
	if (nullptr != m_pMesh)
	{
		AttachComponent("Mesh", m_pMesh);
		m_pMesh->SetTransform(m_pTransform);
		m_pBoundingBox = m_pMesh->GetBoundingBox();
		if (nullptr != m_pBoundingBox)
			m_pBoundingBox->SetTransform(m_pTransform);
	}

	if (nullptr != m_pTransform)
	{
		m_pTransform->SetPosRotScale(vPos, vRot, vScale);
		m_pTransform->Update(0);
	}

	return PK_NOERROR;
}

Character* Character::Create(_uint sTag, _uint lTag, _uint oTag, CLayer* pLayer, string meshID, vec3 vPos, vec3 vRot, vec3 vScale, string soundTag)
{
	Character* pInstance = new Character();
	if (PK_NOERROR != pInstance->Ready(sTag, lTag, oTag, pLayer, meshID, vPos, vRot, vScale, soundTag))
	{
		pInstance->Destroy();
		pInstance = nullptr;
	}

	return pInstance;
}
