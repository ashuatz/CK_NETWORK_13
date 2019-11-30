#pragma once
#include "vector.h"

class Player
{
public:
	int pid;
	float HP;
	float displayHP;
	float lastDisplayHp;

	vector2 PlayerPosition;
	vector2 PlayerSize;

	vector3Int color32;
	vector3Int bulletColor32;

	Player()
	{
		PlayerPosition = vector2(0, 0);
		PlayerSize = vector2(20, 20);
		lastDisplayHp = displayHP = HP = 10000;
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
	vector2 size;

	Player* owner;

	Bullet()
	{
		power = 30; // 120 -1600 // 150 -2500
		moveDirection = vector2(1, 1);

		position = vector2(0,0);
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
	static constexpr float maxGravity = -1000;

	const float GetMaxGravity(float time)
	{
		return abs(Gravity * time) < abs(maxGravity) ? Gravity * time : maxGravity;
	}
};