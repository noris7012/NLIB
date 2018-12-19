#ifndef NLIB_UTILITY_H
#define NLIB_UTILITY_H

#include <stdint.h>
#include <string>

#include "NetworkDefine.h"

class Utility
{
public:
	static uint64_t GetTime();
	static std::string ByteToString(const byte* data, uint32_t length);
	static uint64_t Rand64();
	static uint32_t Rand32();
	static uint32_t IPToInt(const char* ip);
};

#endif