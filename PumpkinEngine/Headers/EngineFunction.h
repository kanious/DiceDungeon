#ifndef _ENGINEFUNCTION_H_
#define _ENGINEFUNCTION_H_

#include <cstdlib>

static int GetRandNum(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

static int GetRandNum_Big(int min, int max)
{
	return ((int)(((double)((rand() << 15) | rand())) / ((RAND_MAX << 15 | RAND_MAX) + 1) * (max - min + 1)) + min);
}

#endif //_ENGINEFUNCTION_H_