#include "Utility.h"

#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>

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

		if (i % 5 == 4)
			ss << " ";
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

double Utility::Rand()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	return static_cast<double>(gen()) / static_cast<double>(UINT32_MAX);
}

byte* Utility::RandBytes(uint32_t length)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	auto bytes = new byte[length];

	int val = 0;

	for (uint32_t i = 0; i < length; ++i)
	{
		if (i % 4 == 0)
		{
			val = gen();
		}

		bytes[i] = byte(val >> 8 * i);
	}

	return bytes;
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


std::string Utility::TimeInHHMMSSMMM()
{
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	return oss.str();
}

std::string Utility::TimeToString(uint64_t time)
{
	auto timer = time_t(time / 1000);

	// convert to broken time
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << time % 1000;

	return oss.str();
}
