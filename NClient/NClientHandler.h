#ifndef TEST_NCLIENT_HANDLER_H
#define TEST_NCLIENT_HANDLER_H

#include "GameClientHandler.h"

class NClientHandler : public GameClientHandler
{
public:
	void HandleConnected(GameClientInterface* session) override;
	void HandlePacket(GameClientInterface* session, GamePacket packet) override;
	void HandleDisconnected(GameClientInterface* session) override;	
};

#endif