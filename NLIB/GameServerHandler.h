#ifndef NLIB_GAME_SERVER_HANDLER_H
#define NLIB_GAME_SERVER_HANDLER_H

#include <memory>

#include "NetworkDefine.h"

class GameServerHandler
{
public:
	virtual void HandleConnected(PGameSession session) = 0;
	virtual void HandlePacket(PGameSession session, PGamePacket packet) = 0;
	virtual void HandleDisconnected(PGameSession session) = 0;
};

#endif