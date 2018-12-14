#include "Utility.h"

#include <chrono>

uint64_t Utility::GetTime()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}