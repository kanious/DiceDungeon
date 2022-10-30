#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "Base.h"
#include "glm/vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CComponent;
class ENGINE_API CXMLParser : public CBase
{
	SINGLETON(CXMLParser)

	struct sShaderdata
	{
		std::string ID;
		std::string PATH_VERTEX;
		std::string PATH_FRAGMENT;
	};

	struct sTexturedata
	{
		std::string ID;
		std::string PATH;
	};

	struct sMeshdata
	{
		std::string ID;
		std::string PATH;
		std::string FILENAME;
		_uint TYPE;
		std::string SHADER_ID;
		std::string TEXTURE_ID_DIFF;
		std::string INITSIZE;
	};

	struct sObjectData
	{
		std::string ID;
		glm::vec3 vPos;
		glm::vec3 vRot;
		glm::vec3 vScale;
	};

private:
	explicit CXMLParser();
	virtual ~CXMLParser();
private:
	void Destroy();

public:
	void LoadShaderData(std::string path);
	void LoadTextureData(std::string path);
	void LoadMeshData(std::string path);
	void LoadSoundData(std::string path);
	void LoadMapObjectData(std::vector<sObjectData>& vec, sObjectData& cameraData, std::string path);
	void SaveMapObjectData(std::vector<sObjectData>& vec, sObjectData& cameraData, std::string path);
};

NAMESPACE_END

#endif //_XMLPARSER_H_