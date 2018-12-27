#ifndef NLIB_GAME_PACKET_H
#define NLIB_GAME_PACKET_H

#include <memory>
#include <cstdint>

#include "NetworkDefine.h"
#include <string>

class GamePacket
{
public:
	static PGamePacket Instance();

public:
	void Set(const byte* bytes, uint32_t length);
	void Print();
	std::string GetString();

private:
	const byte* _bytes;
	uint32_t _length;
};

#endif