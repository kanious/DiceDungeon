#include "MapManager.h"
#include "OpenGLDefines.h"
#include "Define.h"
#include "Scene3D.h"
#include "Rooms.h"
#include "Blocks.h"
#include "Function.h"

SINGLETON_FUNCTION(MapManager)
USING(Engine)
USING(glm)
USING(std)

#define WIDTH 33
#define HEIGHT 16
#define TOTAL WIDTH*HEIGHT

MapManager::MapManager()
	: m_pScene(nullptr)
{
	m_vecBlocks.clear();
	m_vecRooms.clear();
}

MapManager::~MapManager()
{
}

void MapManager::Destroy()
{
	for (int i = 0; i < TOTAL; ++i)
		delete m_vecBlocks[i];
	m_vecBlocks.clear();

	for (int i = 0; i < m_vecRooms.size(); ++i)
		delete m_vecRooms[i];
	m_vecRooms.clear();
}

RESULT MapManager::Ready(Scene3D* pScene)
{
	if (nullptr == pScene)
		return PK_ERROR_NULLPTR;

	m_pScene = pScene;

	m_vecBlocks.resize(TOTAL);
	for (int i = 0; i < TOTAL; ++i)
	{
		m_vecBlocks[i] = new Blocks();
		m_vecBlocks[i]->Ready(i, vec3(i % WIDTH * 5.f, 0.f, i / WIDTH * 5.f));
	}

	SetMapInformation();
	AddTorch();
	CreateBackgroundObjects();
	AddCrystal();

	return PK_NOERROR;
}

void MapManager::SetMapInformation()
{
	// Set Pass
	vector<_uint> vecPassage;
	_uint passIdx = 0;
	passIdx = 2 * WIDTH + 4; vecPassage.push_back(passIdx);
	passIdx = 12 * WIDTH + 4; vecPassage.push_back(passIdx);
	passIdx = 4 * WIDTH + 7; vecPassage.push_back(passIdx);
	passIdx = 10 * WIDTH + 7; vecPassage.push_back(passIdx);
	passIdx = 7 * WIDTH + 12; vecPassage.push_back(passIdx);
	passIdx = 6 * WIDTH + 16; vecPassage.push_back(passIdx);
	passIdx = 12 * WIDTH + 18; vecPassage.push_back(passIdx);
	passIdx = 9 * WIDTH + 19; vecPassage.push_back(passIdx);
	passIdx = 9 * WIDTH + 25; vecPassage.push_back(passIdx);
	passIdx = 6 * WIDTH + 26; vecPassage.push_back(passIdx);
	passIdx = 12 * WIDTH + 26; vecPassage.push_back(passIdx);

	for (int i = 0; i < vecPassage.size(); ++i)
	{
		m_vecBlocks[vecPassage[i]]->m_bIsExist = true;
		m_vecBlocks[vecPassage[i]]->m_bPath = true;
	}


	// Set Rooms
	Rooms* pRoom = nullptr;
	pRoom = new Rooms(); passIdx = 0 * WIDTH + 0; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 4, 5, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 10 * WIDTH + 0; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 4, 5, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 0 * WIDTH + 5; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 5, 4, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 11 * WIDTH + 5; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 5, 4, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 5 * WIDTH + 6; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 6, 5, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 7 * WIDTH + 13; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 6, 5, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 2 * WIDTH + 15; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 3, 4, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 13 * WIDTH + 17; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 3, 3, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 9 * WIDTH + 20; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 5, 3, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 3 * WIDTH + 25; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 3, 3, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 13 * WIDTH + 25; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 3, 3, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);
	pRoom = new Rooms(); passIdx = 7 * WIDTH + 26; pRoom->Ready(&m_vecBlocks, m_vecRooms.size(), 7, 5, WIDTH, HEIGHT, passIdx); m_vecRooms.push_back(pRoom);

	// Set Pass Wall
	for (int i = 0; i < vecPassage.size(); ++i)
	{
		Blocks* pBlock = m_vecBlocks[vecPassage[i]];

		Blocks* nextBlock;

		// up
		nextBlock = m_vecBlocks[vecPassage[i] - WIDTH];
		if (!nextBlock->m_bIsExist)
			pBlock->m_bIsWall[0] = true;

		// left
		nextBlock = m_vecBlocks[vecPassage[i] - 1];
		if (!nextBlock->m_bIsExist)
			pBlock->m_bIsWall[1] = true;

		// right
		nextBlock = m_vecBlocks[vecPassage[i] + 1];
		if (!nextBlock->m_bIsExist)
			pBlock->m_bIsWall[2] = true;

		// down
		nextBlock = m_vecBlocks[vecPassage[i] + WIDTH];
		if (!nextBlock->m_bIsExist)
			pBlock->m_bIsWall[3] = true;
	}
}

void MapManager::AddTorch()
{
	vector<_uint> vecTemp;
	for (int i = 0; i < TOTAL; ++i)
	{
		Blocks* pBlock = m_vecBlocks[i];
		if (!pBlock->m_bIsExist)
			continue;
		if (pBlock->m_bPath)
			continue;

		//if (pBlock->m_bIsWall[0] || pBlock->m_bIsWall[1]
		//	|| pBlock->m_bIsWall[2] || pBlock->m_bIsWall[3])
		if (pBlock->m_bIsWall[0] || pBlock->m_bIsWall[2])
		{
			if (pBlock->m_bIsWall[0] && pBlock->m_bIsWall[1])
				continue;

			if (pBlock->m_bIsWall[0] && pBlock->m_bIsWall[2])
				continue;

			if (pBlock->m_bIsWall[2] && pBlock->m_bIsWall[3])
				continue;

			vecTemp.push_back(i);
		}
	}

	_uint iCount = 0;
	_uint randNum = 0;
	
	while (iCount < 8)
	{
		randNum = GetRandNum(0, vecTemp.size() - 1);
		Blocks* pBlock = m_vecBlocks[vecTemp[randNum]];
		if (pBlock->m_bTorch)
			continue;
		Rooms* room = m_vecRooms[pBlock->m_iRoomIndex];
		if (1 <= room->m_iTorchCount)
			continue;

		pBlock->m_bTorch = true;
		++iCount;
		++room->m_iTorchCount;
	}

	Blocks* pBlock = m_vecBlocks[125];
	pBlock->m_bTorch = true;
}

void MapManager::CreateBackgroundObjects()
{
	vec3 vRot(0.f);
	vec3 vScale(0.01f);
	string floorID = "";
	string wallID = "";
	string torchID = "";

	for (int i = 0; i < TOTAL; ++i)
	{
		Blocks* pBlock = m_vecBlocks[i];
		vec3 vPos = pBlock->m_vCenterPos; 
		
		_int dir = 0;
		vec3 vTorchPos(0.f);
		vec3 vTorchRot(0.f);

		if (pBlock->m_bIsExist)
		{
			floorID = GetRandomFloor();
			m_pScene->AddBGObject(floorID, vPos, vRot, vScale);
		}

		// up
		vPos.z -= 2.5f;
		vRot.y = 180.f;
		if (pBlock->m_bIsDoor[0])
			m_pScene->AddBGObject("DoorWall", vPos, vRot, vScale);
		else if (pBlock->m_bIsWall[0])
		{
			wallID = GetRandomWall();
			m_pScene->AddBGObject(wallID, vPos, vRot, vScale);
			if (pBlock->m_bTorch)
			{
				vTorchPos = vPos;
				vTorchRot = vRot;
				dir = 0;
				if ("Wall01" == wallID)
					vTorchPos.z += 0.2f;
			}
		}

		// left
		vPos = pBlock->m_vCenterPos;
		vPos.x -= 2.5f;
		vRot.y = -90.f;
		if (pBlock->m_bIsDoor[1])
			m_pScene->AddBGObject("DoorWall", vPos, vRot, vScale);
		if (pBlock->m_bIsWall[1])
		{
			wallID = GetRandomWall();
			m_pScene->AddBGObject(wallID, vPos, vRot, vScale);
			//if (pBlock->m_bTorch)
			//{
			//	vTorchPos = vPos;
			//	vTorchRot = vRot;
			//	dir = 1;
			//	if ("Wall01" == wallID)
			//		vTorchPos.x += 0.2f;
			//}
		}

		// right
		vPos = pBlock->m_vCenterPos;
		vPos.x += 2.5f;
		vRot.y = 90.f;
		if (pBlock->m_bIsDoor[2])
			m_pScene->AddBGObject("DoorWall", vPos, vRot, vScale);
		if (pBlock->m_bIsWall[2])
		{
			wallID = GetRandomWall();
			m_pScene->AddBGObject(wallID, vPos, vRot, vScale);
			if (pBlock->m_bTorch)
			{
				vTorchPos = vPos;
				vTorchRot = vRot;
				dir = 2;
				if ("Wall01" == wallID)
					vTorchPos.x -= 0.2f;
			}
		}

		// down
		vPos = pBlock->m_vCenterPos;
		vPos.z += 2.5f;
		vRot.y = 0.f;
		if (pBlock->m_bIsDoor[3])
			m_pScene->AddBGObject("DoorWall", vPos, vRot, vScale);
		if (pBlock->m_bIsWall[3])
		{
			wallID = GetRandomWall();
			m_pScene->AddBGObject(wallID, vPos, vRot, vScale);
			//if (pBlock->m_bTorch)
			//{
			//	vTorchPos = vPos;
			//	vTorchRot = vRot;
			//	dir = 3;
			//	if ("Wall01" == wallID)
			//		vTorchPos.z -= 0.2f;
			//}
		}

		// Torch
		if (pBlock->m_bTorch)
		{
			torchID = GetRandomTorch();
			vTorchPos.y += 2.5f;
			m_pScene->AddBGObject(torchID, vTorchPos, vTorchRot, vScale, dir);
		}
	}
}

void MapManager::AddCrystal()
{
	vec3 vPos = vec3(30.f, 0.f, 60.f);
	vec3 vRot = vec3(0.f);
	vec3 vScale = vec3(0.005f);

 	m_pScene->AddBGObject("Crystal1", vPos, vRot, vScale);

	vPos = vec3(35.f, 0.f, 60.f);
	m_pScene->AddBGObject("Crystal2", vPos, vRot, vScale);

	vPos = vec3(45.f, 0.f, 65.f);
	m_pScene->AddBGObject("Crystal3", vPos, vRot, vScale);
}

std::string MapManager::GetRandomWall()
{
	string str = "Wall02";

	int rand = GetRandNum(1, 100);
	if (rand > 90)
		str = "Wall01";

	return str;
}

std::string MapManager::GetRandomFloor()
{
	string str = "Floor01";

	int rand = GetRandNum(1, 100);
	if (rand > 90)
		str = "Floor02";

	return str;
}

std::string MapManager::GetRandomTorch()
{
	string str = "Torch";

	int rand = GetRandNum(1, 100);
	if (rand > 50)
		str = "Torch2";

	return str;
}
