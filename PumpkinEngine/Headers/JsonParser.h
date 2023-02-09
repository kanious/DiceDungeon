#ifndef _JSONPARSER_H_
#define _JSONPARSER_H_

#include "Base.h"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

NAMESPACE_BEGIN(Engine)

class CComponent;
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

private:
	std::string				m_jsonDataPath;

private:
	explicit CJsonParser();
	virtual ~CJsonParser();
private:
	void Destroy();

public:
	void LoadCharacterList(std::string assetFolderPath, std::string fileName, std::vector<sCharacterData>& vec);
};

NAMESPACE_END

#endif //_JSONPARSER_H_