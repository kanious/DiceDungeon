#include "pch.h"
#include "..\Headers\JsonParser.h"
#include "..\Headers\ComponentMaster.h"
#include "..\Headers\Mesh.h"
#include "..\Headers\Texture.h"
#include "glm\vec3.hpp"

#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include "rapidjson\filereadstream.h"
#include "rapidjson\filewritestream.h"

#include <sstream>


USING(Engine)
USING(std)
USING(glm)
USING(rapidjson)
SINGLETON_FUNCTION(CJsonParser)

CJsonParser::CJsonParser()
{
	m_jsonDataPath = "Json\\";
	m_pCompMaster = CComponentMaster::GetInstance(); m_pCompMaster->AddRefCnt();
}

CJsonParser::~CJsonParser()
{
}

void CJsonParser::Destroy()
{
	SafeDestroy(m_pCompMaster);
}

void CJsonParser::LoadCharacterList(std::string assetFolderPath, std::string fileName, std::vector<sCharacterData>& vec)
{
	Document doc;
	FILE* file;
	LoadDataFromFile(doc, file, assetFolderPath, fileName);
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

	fclose(file);
}

void CJsonParser::LoadTextureData(std::string assetFolderPath, std::string fileName)
{
	Document doc;
	FILE* file;
	LoadDataFromFile(doc, file, assetFolderPath, fileName);
	const Value& dataArray = doc["array"];
	assert(dataArray.IsArray());

	for (int i = 0; i < dataArray.Size(); ++i)
	{
		sTexturedata data;

		const Value& curData = dataArray[i];
		data.ID = curData["ID"].GetString();
		data.PATH = curData["Path"].GetString();
		data.FILENAME = curData["FileName"].GetString();

		stringstream ss;
		ss << assetFolderPath << data.PATH << data.FILENAME;
		CComponent* pComp = CTexture::Create(data.ID, ss.str());

		if (nullptr != pComp)
			m_pCompMaster->AddNewComponent(data.ID, pComp);
	}

	fclose(file);
}

void CJsonParser::LoadMeshData(std::string assetFolderPath, std::string fileName)
{
	Document doc;
	FILE* file;
	LoadDataFromFile(doc, file, assetFolderPath, fileName);
	const Value& dataArray = doc["array"];
	assert(dataArray.IsArray());

	for (int i = 0; i < dataArray.Size(); ++i)
	{
		sMeshData data;

		const Value& curData = dataArray[i];
		data.ID = curData["ID"].GetString();
		data.PATH = curData["Path"].GetString();
		data.FILENAME = curData["FileName"].GetString();
		data.TYPE = curData["Type"].GetInt();
		data.SHADER_ID = curData["Shader_ID"].GetString();
		data.INITSIZE = to_string(curData["InitSize"].GetFloat());
		data.TEXTURE_ID_DIFF = curData["Texture_ID_Diff"].GetString();

		stringstream ss;
		ss << assetFolderPath << data.PATH;
		CComponent* pComp = CMesh::Create(data.ID, ss.str(), data.FILENAME, (ModelType)data.TYPE,
			data.SHADER_ID, data.TEXTURE_ID_DIFF);

		if (nullptr != pComp)
		{
			m_pCompMaster->AddNewComponent(data.ID, pComp);
			m_pCompMaster->AddNewMeshInfo(data.ID, data.INITSIZE);
		}
	}

	fclose(file);
}

void CJsonParser::SaveObjectList(string assetFolderPath, string fileName, vector<sObjectData>& vec, sObjectData& cameraData)
{
	stringstream ss;
	ss << assetFolderPath << m_jsonDataPath << fileName;

	FILE* file;
	fopen_s(&file, ss.str().c_str(), "wb");
	const _uint BUFFER_SIZE = 1000;
	char writeBuffer[BUFFER_SIZE];
	FileWriteStream output(file, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(output);


	char buffer[BUFFER_SIZE];
	int len = 0;

	Document doc;
	doc.SetArray();
	Document::AllocatorType& allocator = doc.GetAllocator();
	for (int i = 0; i < vec.size(); ++i)
	{
		sObjectData curData = vec[i];

		Value obj;
		obj.SetObject();

		Value id;
		len = sprintf_s(buffer, "%s", curData.ID.c_str());
		id.SetString(buffer, len, allocator);
		obj.AddMember("modelName", id, allocator);

		Value texName;
		len = sprintf_s(buffer, "%s", curData.TEXNAME.c_str());
		texName.SetString(buffer, len, allocator);
		obj.AddMember("texName", texName, allocator);

		Value posX;	posX.SetFloat(curData.POSITION.x); obj.AddMember("posX", posX, allocator);
		Value posY;	posY.SetFloat(curData.POSITION.y); obj.AddMember("posY", posY, allocator);
		Value posZ; posZ.SetFloat(curData.POSITION.z); obj.AddMember("posZ", posZ, allocator);

		Value rotX;	rotX.SetFloat(curData.ROTATION.x); obj.AddMember("rotX", rotX, allocator);
		Value rotY;	rotY.SetFloat(curData.ROTATION.y); obj.AddMember("rotY", rotY, allocator);
		Value rotZ; rotZ.SetFloat(curData.ROTATION.z); obj.AddMember("rotZ", rotZ, allocator);

		Value scaleX; scaleX.SetFloat(curData.SCALE.x); obj.AddMember("scaleX", scaleX, allocator);
		Value scaleY; scaleY.SetFloat(curData.SCALE.y); obj.AddMember("scaleY", scaleY, allocator);
		Value scaleZ; scaleZ.SetFloat(curData.SCALE.z); obj.AddMember("scaleZ", scaleZ, allocator);

		doc.PushBack(obj, allocator);
	}

	doc.Accept(writer);
	fclose(file);
}

void CJsonParser::LoadDataFromFile(Document& doc, FILE*& file, string assetFolderPath, string fileName)
{
	stringstream ss;
	ss << assetFolderPath << m_jsonDataPath << fileName;

	fopen_s(&file, ss.str().c_str(), "rb");
	const _uint BUFFER_SIZE = 1000;
	char readBuffer[BUFFER_SIZE];
	FileReadStream input(file, readBuffer, sizeof(readBuffer));
	doc.ParseStream(input);
}
