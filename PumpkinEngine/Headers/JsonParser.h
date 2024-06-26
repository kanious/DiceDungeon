#ifndef _JSONPARSER_H_
#define _JSONPARSER_H_

#include "Base.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "rapidjson/document.h"
#include "fmod/fmod.h"

NAMESPACE_BEGIN(Engine)

class CComponentMaster;

// Save/Load Json files
class ENGINE_API CJsonParser : public CBase
{
	SINGLETON(CJsonParser)

	struct sCharacterData
	{
		std::string MESHID;
		glm::vec3 POSITION;
		glm::vec3 ROTATION;
		glm::vec3 SCALE;
		_uint ANIMTYPE;
		_uint EASETYPE;
		_bool ANIMRANDOM;
	};

	struct sTexturedata
	{
		std::string ID;
		std::string PATH;
		std::string FILENAME;
	};

	struct sMeshData
	{
		std::string ID;
		std::string PATH;
		std::string FILENAME;
		_uint DATATYPE;
		std::string SHADER_ID;
		std::string INITSIZE;
		std::string MESHTYPE;
		_bool LOAD_ASSIMP;
		std::string TEXTURE_ID_DIFF;
		std::string TEXTURE_ID_NORMAL;
	};

	struct sObjectData
	{
		std::string ID;
		std::string LAYERTYPE;
		glm::vec3 POSITION;
		glm::vec3 ROTATION;
		glm::vec3 SCALE;
		_bool LOCK;
		_bool SHOW;
		_bool ALPHA;
	};

	struct sTileData
	{
		_uint ID;
		glm::vec3 POSITION;
		_int NEIGHBORS[8];
	};

private:
	std::string				m_jsonDataPath;
	std::string				m_assetDataPath;
	CComponentMaster*		m_pCompMaster;

private:
	explicit CJsonParser();
	virtual ~CJsonParser();
private:
	void Destroy();

public:
	void SetAssetDataPath(std::string assetFolderPath);
	void LoadCharacterList(std::string fileName, std::vector<sCharacterData>& vec);
	void LoadTextureData(std::string fileName);
	void LoadMeshData(std::string fileName, _bool saveMeshList = false);
	void LoadObjectList(std::string fileName, std::vector<sObjectData>& vec, sObjectData& cameraData);
	void SaveObjectList(std::string fileName, std::vector<sObjectData>& vec, sObjectData& cameraData);
	void SaveTileData(std::string fileName, std::vector<sTileData>& vec);
	void LoadTileData(std::string fileName, std::vector<sTileData>& vec);

private:
	void LoadDataFromFile(rapidjson::Document& doc, FILE*& file, std::string fileName);
};

NAMESPACE_END

#endif //_JSONPARSER_H_