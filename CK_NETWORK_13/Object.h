#pragma once
#include "vector.h"

class Bullet
{
public:
	bool isAlive;
	float gravityTime;
	float power;

	vector2 moveDirection;

	vector2 position;
	vector2 scale;
	vector2 size;

	Bullet()
	{
		power = 50;
		moveDirection = vector2(1, 1);

		position = vector2(0,0);
		scale = vector2(1, 1);
		size = vector2(5, 5);
	}

	void Init(vector2 pos, vector2 dir)
	{
		isAlive = false;

		position = pos;
		moveDirection = dir;
		gravityTime = 0;
	}
};

class Player
{
public:
	int pid;

	vector2 PlayerPosition;
	vector2 PlayerScale;
	vector2 PlayerSize;
};

class World
{
public:
	vector2 defaultWorldOffset;
	vector2 worldOffset;

	static constexpr float Gravity = -9;
	static constexpr float maxGravity = -100;

	const float GetMaxGravity(float time)
	{
		return abs(Gravity * time) < abs(maxGravity) ? Gravity * time : maxGravity;
	}
};