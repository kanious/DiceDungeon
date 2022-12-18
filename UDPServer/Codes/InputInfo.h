#ifndef _INPUTINFO_H_
#define _INPUTINFO_H_

#include "EngineDefines.h"

class InputInfo
{
public:
	_bool forward;
	_bool backward;
	_bool left;
	_bool right;
	_bool turnLeft;
	_bool turnRight;
	_bool fire;
	_bool respawn;

public:
	explicit InputInfo()
		: forward(false), backward(false), left(false), right(false), turnLeft(false), turnRight(false)
		, fire(false), respawn(false)
	{ }
	~InputInfo() { }

public:
	int GetBit(unsigned int num, int digit)
	{
		return (num & (1 << digit)) >> digit;
	}

	void Calculate(int value)
	{
		Reset();

		if (1 == GetBit(value, 0))
			respawn = true;
		if (1 == GetBit(value, 1))
			fire = true;
		if (1 == GetBit(value, 2))
			turnRight = true;
		if (1 == GetBit(value, 3))
			turnLeft = true;
		if (1 == GetBit(value, 4))
			right = true;
		if (1 == GetBit(value, 5))
			left = true;
		if (1 == GetBit(value, 6))
			backward = true;
		if (1 == GetBit(value, 7))
			forward = true;
	}

	int Setting()
	{
		int value = 0;

		if (forward)
			value |= 1 << 7;
		if (backward)
			value |= 1 << 6;
		if (left)
			value |= 1 << 5;
		if (right)
			value |= 1 << 4;
		if (turnLeft)
			value |= 1 << 3;
		if (turnRight)
			value |= 1 << 2;
		if (fire)
			value |= 1 << 1;
		if (respawn)
			value |= 1;

		return value;
	}

	void Reset()
	{
		forward = false;
		backward = false;
		left = false;
		right = false;
		turnLeft = false;
		turnRight = false;
		fire = false;
		respawn = false;
	}
};

#endif //_INPUTINFO_H_