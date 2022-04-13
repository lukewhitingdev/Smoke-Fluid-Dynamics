#pragma once
#include <algorithm>
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
};

