#pragma once
#include <string>
#include <chrono>

class Profile {
public:
	std::string name;
	std::chrono::steady_clock::time_point start;

	Profile() {};
	Profile(std::string name, std::chrono::steady_clock::time_point start) : name(name), start(start) {}
};