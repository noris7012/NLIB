#ifndef NLIB_GAME_SERVER_HANDLER_H
#define NLIB_GAME_SERVER_HANDLER_H

#include <memory>

#include "GameSession.h"
#include "GamePacket.h"

class GameServerHandler
{
public:
	virtual void HandleConnected(PGameSession session) = 0;
	virtual void HandleReceive(PGameSession session, PGamePacket packet) = 0;
	virtual void HandleDisconnected(PGameSession session) = 0;
};

using PGameServerHandler = std::shared_ptr<GameServerHandler>;

#endif