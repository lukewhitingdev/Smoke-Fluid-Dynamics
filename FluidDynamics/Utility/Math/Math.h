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
};

