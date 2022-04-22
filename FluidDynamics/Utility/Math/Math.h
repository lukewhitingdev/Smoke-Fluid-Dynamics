#pragma once
#include <algorithm>
#include <d3d11.h>
#include <DirectXMath.h>

// TODO: Intergrate this with the other classes.
class Vector3
{
public:
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {};
	Vector3(int x, int y, int z) : x(float(x)), y(float(y)), z(float(z)) {};
	Vector3() : x(0), y(0), z(0) {};
	float x, y, z;

	Vector3 operator - (const Vector3& other)
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 operator + (const Vector3& other)
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator * (const Vector3& other)
	{
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	Vector3 operator * (const float& scalar)
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	Vector3 operator + (const float& scalar)
	{
		return Vector3(x + scalar, y + scalar, z + scalar);
	}

	Vector3 operator / (const float& scalar)
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	Vector3	operator -()
	{
		return Vector3(-x, -y, -z);
	};

	operator DirectX::XMFLOAT3()
	{
		return DirectX::XMFLOAT3(x, y, z);
	}
};

class Vector4
{
public:
	Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {};
	Vector4() : x(0), y(0), z(0), w(0) {};
	float x, y, z, w;
};

class Math
{
public:

	// Replacement clamp function for C++ 11, since std::clamp is in c++17.
	// From: https://stackoverflow.com/a/9324086
	static float clamp(const float val, const float min, const float max)
	{
		return std::max<float>(min, std::min<float>(val, max));
	}

	static int clamp(const int val, const int min, int max)
	{
		return std::max<int>(min, std::min<int>(val, max));
	}

	static int random(int min,int max)
	{
		return min + (rand() % static_cast<int>(max - min + 1));
	}

	static bool compareFloat(float a, float b, float accuracy)
	{
		return fabs(a - b) < accuracy;
	}

	static float lerp(float a, float b, float t)
	{
		return a + t * (b-a);
	}

	static Vector3 vecLerp(Vector3 a, Vector3 b, float t)
	{
		return Vector3(lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t));
	}
};

