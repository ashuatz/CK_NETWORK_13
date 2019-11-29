#pragma once
#include "vector.h"

class Player
{
public:
	int pid;

	vector2 PlayerPosition;
	vector2 PlayerScale;
	vector2 PlayerSize;

	vector3Int color32;
	vector3Int bulletColor32;

	Player()
	{
		PlayerPosition = vector2(0, 0);
		PlayerScale = vector2(1, 1);
		PlayerSize = vector2(20, 20);
	}
};

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

	Player* owner;

	Bullet()
	{
		power = 50;
		moveDirection = vector2(1, 1);

		position = vector2(0,0);
		scale = vector2(1, 1);
		size = vector2(5, 5);
	}

	void Init(vector2 targetPoint, Player* const owner)
	{
		this->owner = owner;

		isAlive = false;

		position = owner->PlayerPosition;
		moveDirection = (targetPoint - position).Normalized();
		gravityTime = 0;
	}
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