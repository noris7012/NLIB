#ifndef TEST_NCLIENT_HANDLER_H
#define TEST_NCLIENT_HANDLER_H

#include "GameClientHandler.h"

class NClientHandler : public GameClientHandler
{
public:
	void HandleConnected(PGameClient session) override;
	void HandlePacket(PGameClient session, PGamePacket packet) override;
	void HandleDisconnected(PGameClient session) override;
	
};

#endif