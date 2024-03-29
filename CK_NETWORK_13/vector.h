#pragma once
#include "Matrix.h"

class vector2;
class vector2Int;
class vector3;
class vector3Int;
class vector4;

struct vector2
{
	vector2() : x{ 0.0f }, y{ 0.0f } {}
	vector2(const float &x, const float &y) : x{ x }, y{ y } {}
	vector2(float *arr) : x{ arr[0] }, y{ arr[1] } {}
	vector2(const vector2 &other) : x{ other.x }, y{ other.y } {}
	vector2(const vector2Int &other);
	vector2(const vector3 &other);
	vector2(const vector3Int &other);
	~vector2() = default;

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	vector2 *Normalize()
	{
		float w = sqrt(x * x + y * y);
		x /= w;
		y /= w;
		return this;
	}

	vector2 Normalized() const
	{
		return *(vector2(*this).Normalize());
	}

	vector2 &operator+=(const vector2 &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}
	vector2 &operator-=(const vector2 &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}
	vector2 &operator*=(const vector2 &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}
	vector2 &operator*=(const float &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}
	vector2 &operator*=(const Matrix2x2 &rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	vector2 &operator/=(const vector2 &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}
	vector2 &operator/=(const float &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}
	const vector2 operator+(const vector2 &rhs) const
	{
		return vector2{ this->x + rhs.x, this->y + rhs.y };
	}
	const vector2 operator-(const vector2 &rhs) const
	{
		return vector2{ this->x - rhs.x, this->y - rhs.y };
	}
	const vector2 operator-() const
	{
		return vector2{ -this->x, -this->y };
	}
	const vector2 operator*(const vector2 &rhs) const
	{
		return vector2{ this->x * rhs.x, this->y * rhs.y };
	}
	const vector2 operator*(const float &rhs) const
	{
		return vector2{ this->x * rhs, this->y * rhs };
	}
	const vector2 operator*(const Matrix2x2 &rhs) const
	{
		float t[2]{ 0.0f };
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				t[i] += (*this)[j] * rhs.m[j][i];
			}
		}
		return t;
	}
	const vector2 operator/(const vector2 &rhs) const
	{
		return vector2{ this->x / rhs.x, this->y / rhs.y };
	}
	const vector2 operator/(const float &rhs) const
	{
		return vector2{ this->x / rhs, this->y / rhs };
	}
	friend const vector2 operator*(const float &lhs, const vector2 &rhs)
	{
		return vector2{ rhs.x * lhs, rhs.y * lhs };
	}

public:
	float x, y;
};

struct vector2Int
{
	vector2Int() : x{ 0 }, y{ 0 } {}
	vector2Int(const int &x, const int &y) : x{ x }, y{ y } {}
	vector2Int(const float &x, const float &y) : x{ static_cast<int>(x) }, y{ static_cast<int>(y) } {}
	vector2Int(const vector2Int &other) : x{ other.x }, y{ other.y } {}
	vector2Int(const vector2 &other);
	vector2Int(const vector3 &other);
	vector2Int(const vector3Int &other);
	~vector2Int() = default;

	vector2Int &operator+=(const vector2Int &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}
	vector2Int &operator-=(const vector2Int &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}
	vector2Int &operator*=(const vector2Int &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		return *this;
	}
	vector2Int &operator*=(const float &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}
	vector2Int &operator/=(const vector2Int &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		return *this;
	}
	vector2Int &operator/=(const float &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}
	const vector2Int operator+(const vector2Int &rhs) const
	{
		return vector2Int{ this->x + rhs.x, this->y + rhs.y };
	}
	const vector2Int operator-(const vector2Int &rhs) const
	{
		return vector2Int{ this->x - rhs.x, this->y - rhs.y };
	}
	const vector2Int operator*(const vector2Int &rhs) const
	{
		return vector2Int{ this->x * rhs.x, this->y * rhs.y };
	}
	const vector2Int operator*(const float &rhs) const
	{
		return vector2Int{ this->x * rhs, this->y * rhs };
	}
	const vector2Int operator/(const vector2Int &rhs) const
	{
		return vector2Int{ this->x / rhs.x, this->y / rhs.y };
	}
	const vector2Int operator/(const float &rhs) const
	{
		return vector2Int{ this->x / rhs, this->y / rhs };
	}
	friend const vector2Int operator*(const float &lhs, const vector2Int &rhs)
	{
		return vector2Int{ rhs.x * lhs, rhs.y * lhs };
	}

public:
	int x, y;
};

struct vector3
{
	vector3() : x{ 0.0f }, y{ 0.0f }, z{ 0.0f } {}
	vector3(const float &x, const float &y) : x{ x }, y{ y }, z{ 0.0f } {}
	vector3(const float &x, const float &y, const float &z) : x{ x }, y{ y }, z{ z } {}
	vector3(float *arr) : x{ arr[0] }, y{ arr[1] }, z{ arr[2] } {}
	vector3(const vector3 &other) : x{ other.x }, y{ other.y }, z{ other.z } {}
	vector3(const vector2 &other);
	vector3(const vector2Int &other);
	vector3(const vector3Int &other);
	~vector3() = default;

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	vector3 *Normalize()
	{
		float w = sqrt(x * x + y * y + z * z);
		x /= w;
		y /= w;
		z /= w;
		return this;
	}

	vector3 Normalized()
	{
		return *(vector3(*this).Normalize());
	}

	vector3 &operator+=(const vector3 &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}
	vector3 &operator-=(const vector3 &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}
	vector3 &operator*=(const vector3 &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}
	vector3 &operator*=(const float &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}
	vector3 &operator*=(const Matrix3x3 &rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	vector3 &operator/=(const vector3 &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}
	vector3 &operator/=(const float &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
		return *this;
	}
	const vector3 operator+(const vector3 &rhs) const
	{
		return vector3{ this->x + rhs.x, this->y + rhs.y, this->z + rhs.z };
	}
	const vector3 operator-(const vector3 &rhs) const
	{
		return vector3{ this->x - rhs.x, this->y - rhs.y, this->z - rhs.z };
	}
	const vector3 operator*(const vector3 &rhs) const
	{
		return vector3{ this->x * rhs.x, this->y * rhs.y, this->z * rhs.z };
	}
	const vector3 operator*(const float &rhs) const
	{
		return vector3{ this->x * rhs, this->y * rhs, this->z * rhs };
	}
	const vector3 operator*(const Matrix3x3 &rhs) const
	{
		float t[3]{ 0.0f };
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				t[i] += (*this)[j] * rhs.m[j][i];
			}
		}
		return t;
	}
	const vector3 operator/(const vector3 &rhs) const
	{
		return vector3{ this->x / rhs.x, this->y / rhs.y, this->z / rhs.z };
	}
	const vector3 operator/(const float &rhs) const
	{
		return vector3{ this->x / rhs, this->y / rhs, this->z / rhs };
	}

	//static COLORREF ToColor(const vector3& vector)
	//{
	//	return ((COLORREF)(((BYTE)(vector.z) | ((WORD)((BYTE)(vector.y)) << 8)) | (((DWORD)(BYTE)(vector.x)) << 16)));
	//}

	//static const vector3 ToVector3(const COLORREF& color)
	//{
	//	vector3 v;
	//	v.x = 0 | (color & 255);
	//	v.y = 0 | ((color >> 8) & 255);
	//	v.z = 0 | ((color >> 16) & 255);
	//	return v;
	//}

	friend const vector3 operator*(const float &lhs, const vector3 &rhs)
	{
		return vector3{ rhs.x * lhs, rhs.y * lhs, rhs.z * lhs };
	}

public:
	float x, y, z;
};

struct vector3Int
{
	vector3Int() : x{ 0 }, y{ 0 }, z{ 0 } {}
	vector3Int(const int &x, const int &y) : x{ x }, y{ y }, z{ 0 } {}
	vector3Int(const int &x, const int &y, const int &z) : x{ x }, y{ y }, z{ z } {}
	vector3Int(const float &x, const float &y) : x{ static_cast<int>(x) }, y{ static_cast<int>(y) }, z{ 0 } {}
	vector3Int(const float &x, const float &y, const float &z) : x{ static_cast<int>(x) }, y{ static_cast<int>(y) }, z{ static_cast<int>(z) } {}
	vector3Int(const vector3Int &other) : x{ other.x }, y{ other.y }, z{ other.z } {}
	vector3Int(const vector2 &other);
	vector3Int(const vector2Int &other);
	vector3Int(const vector3 &other);
	~vector3Int() = default;

	vector3Int &operator+=(const vector3Int &rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}
	vector3Int &operator-=(const vector3Int &rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}
	vector3Int &operator*=(const vector3Int &rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}
	vector3Int &operator*=(const float &rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}
	vector3Int &operator/=(const vector3Int &rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}
	vector3Int &operator/=(const float &rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
		return *this;
	}
	const vector3Int operator+(const vector3Int &rhs) const
	{
		return vector3Int{ this->x + rhs.x, this->y + rhs.y, this->z + rhs.z };
	}
	const vector3Int operator-(const vector3Int &rhs) const
	{
		return vector3Int{ this->x - rhs.x, this->y - rhs.y, this->z - rhs.z };
	}
	const vector3Int operator*(const vector3Int &rhs) const
	{
		return vector3Int{ this->x * rhs.x, this->y * rhs.y, this->z * rhs.z };
	}
	const vector3Int operator*(const float &rhs) const
	{
		return vector3Int{ this->x * rhs, this->y * rhs, this->z * rhs };
	}
	const vector3Int operator/(const vector3Int &rhs) const
	{
		return vector3Int{ this->x / rhs.x, this->y / rhs.y, this->z / rhs.z };
	}
	const vector3Int operator/(const float &rhs) const
	{
		return vector3Int{ this->x / rhs, this->y / rhs, this->z / rhs };
	}
	friend const vector3Int operator*(const float &lhs, const vector3Int &rhs)
	{
		return vector3Int{ rhs.x * lhs, rhs.y * lhs, rhs.z * lhs };
	}

public:
	int x, y, z;
};