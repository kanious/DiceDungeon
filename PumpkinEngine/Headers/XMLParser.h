#ifndef _XMLPARSER_H_
#define _XMLPARSER_H_

#include "Base.h"
#include "glm/vec3.hpp"

NAMESPACE_BEGIN(Engine)

class CComponent;
class ENGINE_API CXMLParser : public CBase
{
	SINGLETON(CXMLParser)
	struct sMeshdata
	{
		std::string ID;
		std::string PATH;
		std::string FILENAME;
		_uint TYPE;
		std::string SHADER_ID;
		std::string VERTEXSHADER_PATH;
		std::string FRAGMENTSHADER_PATH;
		std::string INITSIZE;
	};

	struct sShaderdata
	{
		std::string ID;
		std::string PATH_VTX;
		std::string PATH_IDX;
	};

	struct sTexturedata
	{
		std::string ID;
		std::string PATH;
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
	void LoadMeshData(std::vector<sMeshdata>& vector, std::string path);
	void LoadSoundData(std::string path);
	void LoadMapObjectData(std::vector<sObjectData>& vector, std::string path);
};

NAMESPACE_END

#endif //_XMLPARSER_H_