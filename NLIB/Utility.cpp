#include "Utility.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

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

uint64_t Utility::Rand64()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return (((uint64_t)gen()) << 32) + ((uint64_t)gen());
}

uint32_t Utility::Rand32()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return gen();
}