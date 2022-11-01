#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "Base.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

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
		glm::vec3 POSITION;
		glm::vec3 ROTATION;
		glm::vec3 SCALE;
	};

	struct sLightData
	{
		std::string NAME;
		glm::vec4 POSITION;
		glm::vec4 DIRECTION;
		glm::vec4 DIFFUSE;
		glm::vec4 SPECULAR;
		glm::vec4 AMBIENT;
		glm::vec4 ATTEN;
		glm::vec4 PARAM1;
		glm::vec4 PARAM2;
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
	void LoadMapObjectData(std::string path, std::vector<sObjectData>& vec, sObjectData& cameraData);
	void SaveMapObjectData(std::string path, std::vector<sObjectData>& vec, sObjectData& cameraData);
	void LoadLightData(std::string path, std::vector<sLightData>& vec);
	void SaveLightData(std::string path, std::vector<sLightData>& vec);

};

NAMESPACE_END

#endif //_XMLPARSER_H_