#include "Profiler.h"

std::vector<Profile> Profiler::profiles;
std::chrono::milliseconds Profiler::average = std::chrono::milliseconds(0);
std::chrono::milliseconds Profiler::total = std::chrono::milliseconds(0);
std::chrono::milliseconds Profiler::lowest = std::chrono::milliseconds(99999999);
std::chrono::milliseconds Profiler::highest = std::chrono::milliseconds(0);
int Profiler::iterations = 0;

void Profiler::StartProfiling(std::string name)
{
	if (!IsDebuggerPresent())
		return;

	auto timeStart = std::chrono::steady_clock::now();

	Profile foundProfile = Profile();

	for (size_t i = 0; i < profiles.size(); i++)
	{
		Profile profile = profiles[i];
		if (profile.name == name) {
			std::cout << "Tried to start profiling on a ID " << name << " that already exists, continuing profiling on that ID." << std::endl;
			return;
		}
	}

	profiles.push_back(Profile(name, timeStart));
}

void Profiler::StopProfiling(std::string name)
{
	#ifdef __CYGWIN__
		if (!IsDebuggerPresent())
			return;
	#endif

	auto timeEnd = std::chrono::steady_clock::now();
	Profile foundProfile = Profile();
	int index = -1;

	for (size_t i = 0; i < profiles.size(); i++)
	{
		Profile profile = profiles[i];
		if (profile.name == name) {
			foundProfile = profile;
			index = i;
			break;
		}
	}

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - foundProfile.start);

	if (elapsed > highest)
		highest = elapsed;

	if (elapsed < lowest && iterations > 0)
		lowest = elapsed;

	total += elapsed;

	if(iterations > 0)
		average = total / iterations;

	printf("[%s] function completed in %d ms \n [Stats] [R-AVG: %d ms] [H: %d ms] [L: %d ms] \n", name.c_str(), (int)elapsed.count(), (int)average.count(), (int)highest.count(), (int)lowest.count());

	if(index != -1)
		profiles.erase(profiles.begin() + index);

	iterations++;
}
