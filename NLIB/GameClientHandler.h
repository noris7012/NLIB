#ifndef NLIB_GAME_CLIENT_HANDLER_H
#define NLIB_GAME_CLIENT_HANDLER_H

#include <memory>

#include "NetworkDefine.h"

class GameClientHandler
{
public:
	virtual void HandleConnected(PGameClient client) = 0;
	virtual void HandlePacket(PGameClient client, PGamePacket packet) = 0;
	virtual void HandleDisconnected(PGameClient client) = 0;
};
#endif
