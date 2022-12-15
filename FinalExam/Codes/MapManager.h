#ifndef _MAPMANAGER_H_
#define _MAPMANAGER_H_

#include "Base.h"

class Scene3D;
class Blocks;
class Rooms;
class MapManager : public Engine::CBase
{
	SINGLETON(MapManager)
private:
	Scene3D*				m_pScene;
	std::vector<Blocks*>	m_vecBlocks;
	std::vector<Rooms*>		m_vecRooms;

private:
	explicit MapManager();
	~MapManager();
public:
	void Destroy();
	RESULT Ready(Scene3D* pScene);

private:
	void SetMapInformation();
	void AddTorch();
	void CreateBackgroundObjects();
	void AddCrystal();
	std::string GetRandomWall();
	std::string GetRandomFloor();
	std::string GetRandomTorch();
};

#endif //_MAPMANAGER_H_