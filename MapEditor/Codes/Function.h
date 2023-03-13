#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <cstdlib>
#include <glm\vec3.hpp>
#include <string>
#include "Enums.h"

static int GetRandNum(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

static int GetRandNum_Big(int min, int max)
{
	return ((int)(((double)((rand() << 15) | rand())) / ((RAND_MAX << 15 | RAND_MAX) + 1) * (max - min + 1)) + min);
}

static void PrintVector(glm::vec3 vec, std::string title = "")
{
	std::cout << title << ": " << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
}

static std::string GetLayerNameByTag(eLAYERTAG tag)
{
	std::string layerTag = "";
	switch (tag)
	{
	case LAYER_CAMERA:				layerTag = "Layer_camera"; break;
	case LAYER_STATIC_OBJECT:		layerTag = "Layer_static"; break;
	case LAYER_TILE:				layerTag = "Layer_tile"; break;
	case LAYER_INTERACTIVE_OBJECT:	layerTag = "Layer_interactive"; break;
	case LAYER_CHARACTER:			layerTag = "Layer_character"; break;
	case LAYER_ENEMY:				layerTag = "Layer_enemy"; break;
	case LAYER_EVENT_OBJECT:		layerTag = "Layer_event"; break;
	case LAYER_UI:					layerTag = "Layer_ui"; break;
	}
	return layerTag;
}

static eLAYERTAG GetLayerTagByName(std::string name)
{
	if ("Layer_camera" == name)
		return LAYER_CAMERA;
	else if ("Layer_static" == name)
		return LAYER_STATIC_OBJECT;
	else if ("Layer_tile" == name)
		return LAYER_TILE;
	else if ("Layer_interactive" == name)
		return LAYER_INTERACTIVE_OBJECT;
	else if ("Layer_character" == name)
		return LAYER_CHARACTER;
	else if ("Layer_enemy" == name)
		return LAYER_ENEMY;
	else if ("Layer_event" == name)
		return LAYER_EVENT_OBJECT;
	else if ("Layer_ui" == name)
		return LAYER_UI;

	return LAYER_END;
}

#endif //_FUNCTION_H_