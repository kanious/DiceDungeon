#include "pch.h"
#include "..\Headers\JsonParser.h"
#include "glm\vec3.hpp"

#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\filereadstream.h"

#include <sstream>


USING(Engine)
USING(std)
USING(glm)
USING(rapidjson)
SINGLETON_FUNCTION(CJsonParser)

CJsonParser::CJsonParser()
{
	m_jsonDataPath = "Json\\";
}

CJsonParser::~CJsonParser()
{
}

void CJsonParser::Destroy()
{
}

void CJsonParser::LoadCharacterList(std::string assetFolderPath, std::string fileName, std::vector<sCharacterData>& vec)
{
	stringstream ss;
	ss << assetFolderPath << m_jsonDataPath << fileName;

	//여기서 json 불러오기
	FILE* file;
	fopen_s(&file, ss.str().c_str(), "rb");
	const _uint BUFFER_SIZE = 1000;
	char readBuffer[BUFFER_SIZE];

	FileReadStream input(file, readBuffer, sizeof(readBuffer));
	Document doc;
	doc.ParseStream(input);

	const Value& dataArray = doc["array"];
	assert(dataArray.IsArray());
	for (int i = 0; i < dataArray.Size(); ++i)
	{
		sCharacterData data;

		const Value& curData = dataArray[i];
		data.MESHID = curData["meshname"].GetString();
		
		data.POSITION.x = curData["posx"].GetFloat();
		data.POSITION.y = curData["posy"].GetFloat();
		data.POSITION.z = curData["posz"].GetFloat();

		data.ROTATION.x = curData["rotx"].GetFloat();
		data.ROTATION.y = curData["roty"].GetFloat();
		data.ROTATION.z = curData["rotz"].GetFloat();

		data.SCALE.x = curData["scalex"].GetFloat();
		data.SCALE.y = curData["scaley"].GetFloat();
		data.SCALE.z = curData["scalez"].GetFloat();

		data.ANIMTYPE = curData["animtype"].GetInt();
		data.EASETYPE = curData["easetype"].GetInt();
		data.ANIMRANDOM = curData["random"].GetBool();

		vec.push_back(data);
	}


	//여기서 json 불러오기

	//xml_document xmlData;
	//xml_parse_result result = xmlData.load_file(ss.str().c_str());
	//if (!result)
	//	return;

	//xml_object_range<xml_node_iterator> list = xmlData.child("Object_Data").children();
	//if (list.empty())
	//	return;

	//xml_node_iterator iter;
	//for (iter = list.begin(); iter != list.end(); ++iter)
	//{
	//	xml_node node = *iter;
	//	xml_attribute category = node.attribute("category");
	//	_bool isCamera = !strcmp(category.value(), "Camera");
	//	xml_node_iterator childIter;

	//	sObjectData data;
	//	for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
	//	{
	//		xml_node childNode = *childIter;
	//		if (isCamera)
	//		{
	//			if (!strcmp(childNode.name(), "EyePosX"))
	//				cameraData.POSITION.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "EyePosY"))
	//				cameraData.POSITION.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "EyePosZ"))
	//				cameraData.POSITION.z = (_float)atof(childNode.child_value());

	//			if (!strcmp(childNode.name(), "EyeRotX"))
	//				cameraData.ROTATION.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "EyeRotY"))
	//				cameraData.ROTATION.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "EyeRotZ"))
	//				cameraData.ROTATION.z = (_float)atof(childNode.child_value());

	//			if (!strcmp(childNode.name(), "TargetPosX"))
	//				cameraData.SCALE.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "TargetPosY"))
	//				cameraData.SCALE.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "TargetPosZ"))
	//				cameraData.SCALE.z = (_float)atof(childNode.child_value());
	//		}
	//		else
	//		{
	//			if (!strcmp(childNode.name(), "ID"))
	//				data.ID = childNode.child_value();
	//			if (!strcmp(childNode.name(), "PosX"))
	//				data.POSITION.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "PosY"))
	//				data.POSITION.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "PosZ"))
	//				data.POSITION.z = (_float)atof(childNode.child_value());

	//			if (!strcmp(childNode.name(), "RotX"))
	//				data.ROTATION.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "RotY"))
	//				data.ROTATION.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "RotZ"))
	//				data.ROTATION.z = (_float)atof(childNode.child_value());

	//			if (!strcmp(childNode.name(), "ScaleX"))
	//				data.SCALE.x = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "ScaleY"))
	//				data.SCALE.y = (_float)atof(childNode.child_value());
	//			if (!strcmp(childNode.name(), "ScaleZ"))
	//				data.SCALE.z = (_float)atof(childNode.child_value());

	//			if (!strcmp(childNode.name(), "EffectSound"))
	//				data.SOUNDTAG = childNode.child_value();
	//			if (!strcmp(childNode.name(), "Lock"))
	//				data.LOCK = (_bool)atoi(childNode.child_value());
	//		}
	//	}
	//	if (!isCamera)
	//		vec.push_back(data);
	//}
}
