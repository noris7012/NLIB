#include "Utility.h"

#include <chrono>
#include <sstream>
#include <iomanip>

uint64_t Utility::GetTime()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

std::string Utility::ByteToString(const byte* data, uint32_t length)
{
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i = 0; i < length; ++i)
	{
		ss << std::setw(2) << static_cast<unsigned>(data[i]);
	}

	return ss.str();
}