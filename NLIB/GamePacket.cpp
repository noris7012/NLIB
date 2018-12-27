#include "GamePacket.h"
#include <iostream>

PGamePacket GamePacket::Instance()
{
	return std::make_shared<GamePacket>();
}

void GamePacket::Set(const byte* bytes, uint32_t length)
{
	_bytes = bytes;
	_length = length;
}

void GamePacket::Print()
{
	std::string str(reinterpret_cast<const char*>(_bytes), _length);

	std::cout << "[GamePacket] " << str.c_str() << std::endl;
}

std::string GamePacket::GetString()
{
	return std::string(reinterpret_cast<const char*>(_bytes), _length - 1);
}
