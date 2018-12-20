#ifndef TEST_NSERVER_HANDLER_H
#define TEST_NSERVER_HANDLER_H

#include "GameServerHandler.h"

class NServerHandler : public GameServerHandler
{
public:
	void HandleConnected(PGameSession session) override;
	void HandlePacket(PGameSession session, PGamePacket packet) override;
	void HandleDisconnected(PGameSession session) override;
};

#endif