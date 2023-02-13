#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <cstdlib>
#include <glm\vec3.hpp>
#include <string>

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

#endif //_FUNCTION_H_