#ifndef NLIB_GAME_SERVER_HANDLER_H
#define NLIB_GAME_SERVER_HANDLER_H

#include <memory>

#include "NetworkDefine.h"

class GameSessionInterface;

class GameServerHandler
{
public:
	virtual ~GameServerHandler() = default;

public:
	virtual void HandleConnected(GameSessionInterface* session) = 0;
	virtual void HandlePacket(GameSessionInterface* session, GamePacket packet) = 0;
	virtual void HandleDisconnected(GameSessionInterface* session) = 0;
};

#endif