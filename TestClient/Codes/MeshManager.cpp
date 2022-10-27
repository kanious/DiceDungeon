#include "MeshManager.h"

SINGLETON_FUNCTION(MeshManager)
USING(Engine)

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}

void MeshManager::Destroy()
{
	DestroyInstance();
}

void MeshManager::LoadMeshFromFile()
{
}

CMesh* MeshManager::CloneMeshComponent(const char* name)
{
	return nullptr;
}
