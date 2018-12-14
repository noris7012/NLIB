#include "Utility.h"

#include <chrono>

uint64_t Utility::GetTime()
{
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

std::string Utility::ByteToString(const byte* data, uint32_t length)
{
	char* arr = new char[length * 2 + 1];
	for (int i = 0; i < length; ++i)
	{
		sprintf_s(arr + i * 2, length * 2 + 1, "%02x", data[i]);
	}
	arr[length * 2] = 0;

	auto ret = std::string(arr);

	return ret;
}