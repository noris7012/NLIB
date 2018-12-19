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
	for (uint32_t i = 0; i < length; ++i)
	{
		ss << std::setw(2) << static_cast<unsigned>(data[i]);
	}

	return ss.str();
}

uint64_t Utility::Rand64()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return (uint64_t(gen()) << 32) + uint64_t(gen());
}

uint32_t Utility::Rand32()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return gen();
}

uint32_t Utility::IPToInt(const char* ip)
{
	uint32_t byte1, byte2, byte3, byte4;
	char dot;
	std::istringstream s(ip);

	s >> byte1 >> dot >> byte2 >> dot >> byte3 >> dot >> byte4 >> dot;

	uint32_t ret = 0;
	ret += byte1 << 8 * 3;
	ret += byte2 << 8 * 2;
	ret += byte3 << 8 * 1;
	ret += byte4 << 8 * 0;

	return ret;
}
