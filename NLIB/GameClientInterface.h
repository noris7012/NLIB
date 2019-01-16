#ifndef NLIB_GAME_CLIENT_INTERFACE_H
#define NLIB_GAME_CLIENT_INTERFACE_H

#include <climits>

#include "NetworkDefine.h"
#include "GameConfig.h"

class GameClientInterface
{
public:
	virtual void WritePacket(const byte* bytes, uint32_t length) = 0;

	virtual bool Connect(GameConfig& config) = 0;
	virtual bool IsConnected() = 0;
	virtual void Disconnect() = 0;
};

#endif