#ifndef _MESHMANAGER_H_
#define _MESHMANAGER_H_

#include "Define.h"

namespace Engine
{
	class CMesh;
}

class MeshManager
{
	SINGLETON(MeshManager)

private:
	explicit MeshManager();
	~MeshManager();

public:
	void Destroy();

public:
	void LoadMeshFromFile();
	Engine::CMesh* CloneMeshComponent(const char* name);
};

#endif //_MESHMANAGER_H_