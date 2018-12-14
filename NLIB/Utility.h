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
};

#endif