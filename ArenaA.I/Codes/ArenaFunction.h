#ifndef _ARENAFUNCTION_H_
#define _ARENAFUNCTION_H_

#include <cstdlib>

static int GetRandNum(int min, int max)
{
	return (rand() % (max - min + 1)) + min;
}

#endif //_ARENAFUNCTION_H_