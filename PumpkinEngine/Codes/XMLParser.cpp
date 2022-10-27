#include "pch.h"
#include "..\Headers\XMLParser.h"
#include "..\Headers\SoundMaster.h"
#include <pugixml/pugixml.hpp>


USING(Engine)
USING(pugi)
USING(std)
SINGLETON_FUNCTION(CXMLParser)

CXMLParser::CXMLParser()
{
}

CXMLParser::~CXMLParser()
{
}

void CXMLParser::Destroy()
{
}

void CXMLParser::LoadMeshData(std::vector<sMeshdata>& vector, std::string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Mesh_List").children();
	if (list.empty())
		return;

	xml_node_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		xml_node node = *iter;
		xml_node_iterator childIter;
		
		sMeshdata data;
		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (!strcmp(childNode.name(), "ID"))
				data.ID = childNode.child_value();
			if (!strcmp(childNode.name(), "Path"))
				data.PATH = childNode.child_value();
			if (!strcmp(childNode.name(), "FileName"))
				data.FILENAME = childNode.child_value();
			if (!strcmp(childNode.name(), "Type"))
				data.TYPE = (_uint)atoi(childNode.child_value());
			if (!strcmp(childNode.name(), "Shader_ID"))
				data.SHADER_ID = childNode.child_value();
			if (!strcmp(childNode.name(), "VertexShaderPath"))
				data.VERTEXSHADER_PATH = childNode.child_value();
			if (!strcmp(childNode.name(), "FragmentShaderPath"))
				data.FRAGMENTSHADER_PATH = childNode.child_value();
			if (!strcmp(childNode.name(), "InitSize"))
				data.INITSIZE = childNode.child_value();
		}
		vector.push_back(data);
	}
}

void CXMLParser::LoadSoundData(std::string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Sound_Data").children();
	if (list.empty())
		return;

	CSoundMaster* pInstance = CSoundMaster::GetInstance();

	xml_node_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		xml_node node = *iter;
		xml_attribute category = node.attribute("category");
		xml_node_iterator childIter;

		_int channelCount = 0;
		string channelTag = "";
		string soundTag = "";
		string soundPath = "";
		string soundChannelTag = "";
		_bool loop = false;
		string dspTag = "";
		_int type = 0;

		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (!strcmp(category.value(), "MaxChannelCount"))
			{
				if (!strcmp(childNode.name(), "Count"))
					channelCount = (_int)atoi(childNode.child_value());
			}
			else if (!strcmp(category.value(), "ChannelGroup"))
			{
				if (!strcmp(childNode.name(), "Tag"))
					channelTag = childNode.child_value();
			}
			else if (!strcmp(category.value(), "SoundFile"))
			{
				if (!strcmp(childNode.name(), "Tag"))
					soundTag = childNode.child_value();
				else if (!strcmp(childNode.name(), "Path"))
					soundPath = childNode.child_value();
				else if (!strcmp(childNode.name(), "ChannelTag"))
					soundChannelTag = childNode.child_value();
				else if (!strcmp(childNode.name(), "Loop"))
					loop = (_bool)atoi(childNode.child_value());
			}
			else if (!strcmp(category.value(), "DSP"))
			{
				if (!strcmp(childNode.name(), "Tag"))
					dspTag = childNode.child_value();
				else if (!strcmp(childNode.name(), "Type"))
					type = atoi(childNode.child_value());
			}
		}

		if (!strcmp(category.value(), "MaxChannelCount"))
			pInstance->Ready(channelCount);
		else if (!strcmp(category.value(), "ChannelGroup"))
			pInstance->CreateChannelGroup(channelTag);
		else if (!strcmp(category.value(), "SoundFile"))
		{
			if (!loop)
				pInstance->LoadSound(soundTag, soundPath, soundChannelTag);
			else
				pInstance->LoadLoopSound(soundTag, soundPath, soundChannelTag);
		}
		else if (!strcmp(category.value(), "DSP"))
			pInstance->CreateDSPEffect(dspTag, type);
	}
}

void CXMLParser::LoadMapObjectData(std::vector<sObjectData>& vector, std::string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Object_Data").children();
	if (list.empty())
		return;

	xml_node_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		xml_node node = *iter;
		xml_node_iterator childIter;

		sObjectData data;
		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (!strcmp(childNode.name(), "ID"))
				data.ID = childNode.child_value();
			if (!strcmp(childNode.name(), "PosX"))
				data.vPos.x = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "PosY"))
				data.vPos.y = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "PosZ"))
				data.vPos.z = (_float)atof(childNode.child_value());

			if (!strcmp(childNode.name(), "RotX"))
				data.vRot.x = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "RotY"))
				data.vRot.y = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "RotZ"))
				data.vRot.z = (_float)atof(childNode.child_value());

			if (!strcmp(childNode.name(), "ScaleX"))
				data.vScale.x = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "ScaleY"))
				data.vScale.y = (_float)atof(childNode.child_value());
			if (!strcmp(childNode.name(), "ScaleZ"))
				data.vScale.z = (_float)atof(childNode.child_value());
		}
		vector.push_back(data);
	}
}
