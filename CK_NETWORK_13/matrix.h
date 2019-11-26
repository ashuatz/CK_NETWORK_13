#pragma once
#include <cmath>

struct Matrix2x2
{
	Matrix2x2() : m{ 0.0f, }
	{
	}
	Matrix2x2(float _11, float _12, float _21, float _22)
		: _11{ _11 }, _12{ _12 }, _21{ _21 }, _22{ _22 } {}
	Matrix2x2(float *arr) : m{
								0.0f,
	}
	{
		for (int i = 0; i < 4; i++)
		{
			(*this)[i] = arr[i];
		}
	}
	Matrix2x2(const Matrix2x2 &other)
		: _11{ other._11 }, _12{ other._12 }, _21{ other._21 }, _22{ other._22 } {}
	~Matrix2x2() {}

	operator float *() { return *m; }
	operator const float *() const { return *m; }

	Matrix2x2 &operator+=(const Matrix2x2 &rhs)
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				m[i][j] += rhs.m[i][j];
			}
		}
	}
	Matrix2x2 &operator-=(const Matrix2x2 &rhs)
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				m[i][j] -= rhs.m[i][j];
			}
		}
	}
	Matrix2x2 &operator*=(const Matrix2x2 &rhs)
	{
		*this = *this * rhs;
	}
	const Matrix2x2 operator+(const Matrix2x2 &rhs) const
	{
		return Matrix2x2{
			_11 + rhs._11, _12 + rhs._12,
			_21 + rhs._21, _22 + rhs._22 };
	}
	const Matrix2x2 operator-(const Matrix2x2 &rhs) const
	{
		return Matrix2x2{
			_11 - rhs._11, _12 - rhs._12,
			_21 - rhs._21, _22 - rhs._22 };
	}
	const Matrix2x2 operator*(const Matrix2x2 &rhs) const
	{
		Matrix2x2 tmp{};
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					tmp.m[i][j] += m[i][k] * rhs.m[k][j];
				}
			}
		}
		return tmp;
	}

public:
	union {
		struct
		{
			float _11, _12;
			float _21, _22;
		};
		float m[2][2];
	};
};

struct Matrix3x3
{
	Matrix3x3() : m{ 0.0f } {}
	Matrix3x3(
		float _11, float _12, float _13,
		float _21, float _22, float _23,
		float _31, float _32, float _33)
		: _11{ _11 }, _12{ _12 }, _13{ _13 },
		_21{ _21 }, _22{ _22 }, _23{ _23 },
		_31{ _31 }, _32{ _32 }, _33{ _33 } {}
	Matrix3x3(float *arr) : m{ 0.0f }
	{
		for (int i = 0; i < 9; i++)
		{
			(*this)[i] = arr[i];
		}
	}
	Matrix3x3(const Matrix3x3 &other)
		: _11{ other._11 }, _12{ other._12 }, _13{ other._13 },
		_21{ other._21 }, _22{ other._22 }, _23{ other._23 },
		_31{ other._31 }, _32{ other._32 }, _33{ other._33 } {}
	~Matrix3x3() {}

	operator float *() { return *m; }
	operator const float *() const { return *m; }

	Matrix3x3 &operator+=(const Matrix3x3 &rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m[i][j] += rhs.m[i][j];
			}
		}
	}
	Matrix3x3 &operator-=(const Matrix3x3 &rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				m[i][j] -= rhs.m[i][j];
			}
		}
	}
	Matrix3x3 &operator*=(const Matrix3x3 &rhs)
	{
		*this = *this * rhs;
		return *this;
	}
	const Matrix3x3 operator+(const Matrix3x3 &rhs) const
	{
		return Matrix3x3{
			_11 + rhs._11, _12 + rhs._12, _13 + rhs._13,
			_21 + rhs._21, _22 + rhs._22, _23 + rhs._23,
			_31 + rhs._31, _32 + rhs._32, _33 + rhs._33 };
	}
	const Matrix3x3 operator-(const Matrix3x3 &rhs) const
	{
		return Matrix3x3{
			_11 - rhs._11, _12 - rhs._12, _13 - rhs._13,
			_21 - rhs._21, _22 - rhs._22, _23 - rhs._23,
			_31 - rhs._31, _32 - rhs._32, _33 - rhs._33 };
	}
	const Matrix3x3 operator*(const Matrix3x3 &rhs) const
	{
		Matrix3x3 tmp{};
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					tmp.m[i][j] += m[i][k] * rhs.m[k][j];
				}
			}
		}
		return tmp;
	}

public:
	union {
		struct
		{
			float _11, _12, _13;
			float _21, _22, _23;
			float _31, _32, _33;
		};
		float m[3][3];
	};
};


inline Matrix2x2 &MatrixIdentity(Matrix2x2 &matrix)
{
	matrix.m[0][1] = matrix.m[1][0] = 0.0f;

	matrix.m[0][0] = matrix.m[1][1] = 1.0f;

	return matrix;
}

inline Matrix3x3 &MatrixIdentity(Matrix3x3 &matrix)
{
	matrix.m[0][1] = matrix.m[0][2] =
		matrix.m[1][0] = matrix.m[1][2] =
		matrix.m[2][0] = matrix.m[2][1] = 0.0f;

	matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 1.0f;

	return matrix;
}

inline Matrix3x3 &MatrixTranslation(Matrix3x3 &matrix, const float &x, const float &y)
{
	MatrixIdentity(matrix);

	matrix.m[2][0] = x;
	matrix.m[2][1] = y;

	return matrix;
}


inline Matrix3x3 &MatrixScale(Matrix3x3 &matrix, const float &x, const float &y)
{
	MatrixIdentity(matrix);

	matrix.m[0][0] = x;
	matrix.m[1][1] = y;

	return matrix;
}

inline Matrix3x3 &MatrixRotationDir(Matrix3x3 &matrix, const float &dir)
{
	MatrixIdentity(matrix);

	matrix.m[0][0] = cosf(dir);
	matrix.m[0][1] = sinf(dir);
	matrix.m[1][0] = -sinf(dir);
	matrix.m[1][1] = cosf(dir);

	return matrix;
}