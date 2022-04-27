#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <vector>
#include <Windows.h>
#include <debugapi.h>
#include "Profile.h"


const bool useProfiling = false;

class Profiler
{
private:
	static std::vector<Profile> profiles;
	static std::chrono::milliseconds average;
	static std::chrono::milliseconds total;
	static std::chrono::milliseconds highest;
	static std::chrono::milliseconds lowest;
	static int iterations;

public:
	Profiler() {};
	static void StartProfiling(std::string name);
	static void StopProfiling(std::string name);
};

