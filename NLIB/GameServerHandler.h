#ifndef NLIB_GAME_SERVER_HANDLER_H
#define NLIB_GAME_SERVER_HANDLER_H

#include <memory>

#include "GameSession.h"
#include "GamePacket.h"

class GameSession;

class GameServerHandler
{
public:
	virtual void HandleConnected(std::shared_ptr<GameSession> session) = 0;
	virtual void HandlePacket(std::shared_ptr<GameSession> session, PGamePacket packet) = 0;
	virtual void HandleDisconnected(std::shared_ptr<GameSession> session) = 0;
};

using PGameServerHandler = std::shared_ptr<GameServerHandler>;

#endif