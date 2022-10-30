#include "pch.h"
#include "..\Headers\XMLParser.h"
#include "..\Headers\ComponentMaster.h"
#include "..\Headers\Component.h"
#include "..\Headers\Mesh.h"
#include "..\Headers\Shader.h"
#include "..\Headers\Texture.h"
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

void CXMLParser::LoadShaderData(string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Shader_List").children();
	if (list.empty())
		return;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	xml_node_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		xml_node node = *iter;
		xml_node_iterator childIter;

		sShaderdata data;
		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (!strcmp(childNode.name(), "ID"))
				data.ID = childNode.child_value();
			if (!strcmp(childNode.name(), "VertexPath"))
				data.PATH_VERTEX = childNode.child_value();
			if (!strcmp(childNode.name(), "FragmentPath"))
				data.PATH_FRAGMENT= childNode.child_value();
		}

		pComponent = CShader::Create(data.ID, data.PATH_VERTEX.c_str(), data.PATH_FRAGMENT.c_str());
		if (nullptr != pComponent)
			pMaster->AddNewComponent(data.ID, pComponent);
	}
}

void CXMLParser::LoadTextureData(string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Texture_List").children();
	if (list.empty())
		return;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

	xml_node_iterator iter;
	for (iter = list.begin(); iter != list.end(); ++iter)
	{
		xml_node node = *iter;
		xml_node_iterator childIter;

		sTexturedata data;
		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (!strcmp(childNode.name(), "ID"))
				data.ID = childNode.child_value();
			if (!strcmp(childNode.name(), "Path"))
				data.PATH = childNode.child_value();
		}

		pComponent = CTexture::Create(data.ID, data.PATH);
		if (nullptr != pComponent)
			pMaster->AddNewComponent(data.ID, pComponent);
	}
}

void CXMLParser::LoadMeshData(string path)
{
	xml_document xmlData;
	xml_parse_result result = xmlData.load_file(path.c_str());
	if (!result)
		return;

	xml_object_range<xml_node_iterator> list = xmlData.child("Mesh_List").children();
	if (list.empty())
		return;

	CComponentMaster* pMaster = CComponentMaster::GetInstance();
	CComponent* pComponent = nullptr;

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
			if (!strcmp(childNode.name(), "Texture_ID_Diff"))
				data.TEXTURE_ID_DIFF = childNode.child_value();
			if (!strcmp(childNode.name(), "InitSize"))
				data.INITSIZE = childNode.child_value();
		}

		pComponent = CMesh::Create(data.ID, data.PATH, data.FILENAME, (ModelType)data.TYPE,
			data.SHADER_ID, data.TEXTURE_ID_DIFF);
		if (nullptr != pComponent)
		{
			pMaster->AddNewComponent(data.ID, pComponent);
			pMaster->AddNewMeshInfo(data.ID, data.INITSIZE);
		}
	}
}

void CXMLParser::LoadSoundData(string path)
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

void CXMLParser::LoadMapObjectData(vector<sObjectData>& vec, sObjectData& cameraData, string path)
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
		xml_attribute category = node.attribute("category");
		_bool isCamera = !strcmp(category.value(), "Camera");
		xml_node_iterator childIter;

		sObjectData data;
		for (childIter = node.children().begin(); childIter != node.children().end(); ++childIter)
		{
			xml_node childNode = *childIter;
			if (isCamera)
			{
				if (!strcmp(childNode.name(), "EyePosX"))
					cameraData.vPos.x = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "EyePosY"))
					cameraData.vPos.y = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "EyePosZ"))
					cameraData.vPos.z = (_float)atof(childNode.child_value());

				if (!strcmp(childNode.name(), "EyeRotX"))
					cameraData.vRot.x = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "EyeRotY"))
					cameraData.vRot.y = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "EyeRotZ"))
					cameraData.vRot.z = (_float)atof(childNode.child_value());

				if (!strcmp(childNode.name(), "TargetPosX"))
					cameraData.vScale.x = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "TargetPosY"))
					cameraData.vScale.y = (_float)atof(childNode.child_value());
				if (!strcmp(childNode.name(), "TargetPosZ"))
					cameraData.vScale.z = (_float)atof(childNode.child_value());
			}
			else
			{
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
		}
		if (!isCamera)
			vec.push_back(data);
	}
}

void CXMLParser::SaveMapObjectData(vector<sObjectData>& vec, sObjectData& cameraData, string path)
{
	xml_document xmlData;
	//xml_node root = xmlData.document_element();

	xml_node declareNode = xmlData.append_child(node_declaration);
	declareNode.append_attribute("version") = "1.0";
	declareNode.append_attribute("encoding") = "UTF-8";

	xml_node element = xmlData.append_child("Object_Data");

	xml_node nodeCamera = element.append_child("Objects");
	nodeCamera.append_attribute("category") = "Camera";

	xml_node childCameraex = nodeCamera.append_child("EyePosX");
	childCameraex.append_child(node_pcdata).set_value(to_string(cameraData.vPos.x).c_str());
	xml_node childCameraey = nodeCamera.append_child("EyePosY");
	childCameraey.append_child(node_pcdata).set_value(to_string(cameraData.vPos.y).c_str());
	xml_node childCameraez = nodeCamera.append_child("EyePosZ");
	childCameraez.append_child(node_pcdata).set_value(to_string(cameraData.vPos.z).c_str());

	xml_node childCamerarx = nodeCamera.append_child("EyeRotX");
	childCamerarx.append_child(node_pcdata).set_value(to_string(cameraData.vRot.x).c_str());
	xml_node childCamerary = nodeCamera.append_child("EyeRotY");
	childCamerary.append_child(node_pcdata).set_value(to_string(cameraData.vRot.y).c_str());
	xml_node childCamerarz = nodeCamera.append_child("EyeRotZ");
	childCamerarz.append_child(node_pcdata).set_value(to_string(cameraData.vRot.z).c_str());

	xml_node childCameratx = nodeCamera.append_child("TargetPosX");
	childCameratx.append_child(node_pcdata).set_value(to_string(cameraData.vScale.x).c_str());
	xml_node childCameraty = nodeCamera.append_child("TargetPosY");
	childCameraty.append_child(node_pcdata).set_value(to_string(cameraData.vScale.y).c_str());
	xml_node childCameratz = nodeCamera.append_child("TargetPosZ");
	childCameratz.append_child(node_pcdata).set_value(to_string(cameraData.vScale.z).c_str());

	vector<sObjectData>::iterator iter;
	for (iter = vec.begin(); iter != vec.end(); ++iter)
	{
		xml_node nodeChild = element.append_child("Objects");
		xml_node child = nodeChild.append_child("ID");
		child.append_child(node_pcdata).set_value(iter->ID.c_str());
		xml_node childpx = nodeChild.append_child("PosX");
		childpx.append_child(node_pcdata).set_value(to_string(iter->vPos.x).c_str());
		xml_node childpy = nodeChild.append_child("PosY");
		childpy.append_child(node_pcdata).set_value(to_string(iter->vPos.y).c_str());
		xml_node childpz = nodeChild.append_child("PosZ");
		childpz.append_child(node_pcdata).set_value(to_string(iter->vPos.z).c_str());

		xml_node childrx = nodeChild.append_child("RotX");
		childrx.append_child(node_pcdata).set_value(to_string(iter->vRot.x).c_str());
		xml_node childry = nodeChild.append_child("RotY");
		childry.append_child(node_pcdata).set_value(to_string(iter->vRot.y).c_str());
		xml_node childrz = nodeChild.append_child("RotZ");
		childrz.append_child(node_pcdata).set_value(to_string(iter->vRot.z).c_str());

		xml_node childsx = nodeChild.append_child("ScaleX");
		childsx.append_child(node_pcdata).set_value(to_string(iter->vScale.x).c_str());
		xml_node childsy = nodeChild.append_child("ScaleY");
		childsy.append_child(node_pcdata).set_value(to_string(iter->vScale.y).c_str());
		xml_node childsz = nodeChild.append_child("ScaleZ");
		childsz.append_child(node_pcdata).set_value(to_string(iter->vScale.z).c_str());
	}

	_bool result = xmlData.save_file(path.c_str());



	//pugi::xml_document doc;

	//pugi::xml_node root = doc.document_element();

	//auto declarationNode = doc.append_child(pugi::node_declaration);
	//declarationNode.append_attribute("version") = "1.0";
	//declarationNode.append_attribute("encoding") = "UTF-8";
	//declarationNode.append_attribute("standalone") = "yes";


	//pugi::xml_node  element = doc.append_child("file");

	//pugi::xml_node nodeChild = element.append_child("Hello");

	//nodeChild.append_child(pugi::node_pcdata).set_value("World");

	//pugi::xml_node nodeChild1 = element.append_child("Happy");

	//nodeChild1.append_child(pugi::node_pcdata).set_value("India");

	//bool saveSucceeded = doc.save_file("Demo.xml");
}
