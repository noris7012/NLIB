#ifndef NLIB_GAME_CLIENT_HANDLER_H
#define NLIB_GAME_CLIENT_HANDLER_H

#include <memory>

#include "NetworkDefine.h"

class GameClientInterface;

class GameClientHandler
{
public:
	virtual ~GameClientHandler() = default;

public:
	virtual void HandleConnected(GameClientInterface* client) = 0;
	virtual void HandlePacket(GameClientInterface* client, GamePacket packet) = 0;
	virtual void HandleDisconnected(GameClientInterface* client) = 0;
};
#endif
