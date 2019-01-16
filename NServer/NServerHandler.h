#ifndef TEST_NSERVER_HANDLER_H
#define TEST_NSERVER_HANDLER_H

#include "GameServerHandler.h"

class NServerHandler : public GameServerHandler
{
public:
	void HandleConnected(GameSessionInterface* session) override;
	void HandlePacket(GameSessionInterface* session, GamePacket packet) override;
	void HandleDisconnected(GameSessionInterface* session) override;
};

#endif