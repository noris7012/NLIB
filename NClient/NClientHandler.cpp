#include "NClientHandler.h"

#include <iostream>

#include "GamePacket.h"

void NClientHandler::HandleConnected(GameClientInterface* session)
{
	std::cout << "[Client Connected]" << std::endl;
}

void NClientHandler::HandlePacket(GameClientInterface* session, GamePacket packet)
{
#ifdef NLIB_LOG_ENABLED
	packet->Print();
#endif
}

void NClientHandler::HandleDisconnected(GameClientInterface* session)
{
	std::cout << "[Client Disconnected]" << std::endl;
}
