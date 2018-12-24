#include "NClientHandler.h"

#include <iostream>

#include "GamePacket.h"

void NClientHandler::HandleConnected(PGameClient session)
{
	std::cout << "[Client Connected]" << std::endl;
}

void NClientHandler::HandlePacket(PGameClient session, PGamePacket packet)
{
#ifdef NLIB_LOG_ENABLED
	packet->Print();
#endif
}

void NClientHandler::HandleDisconnected(PGameClient session)
{
	std::cout << "[Client Disconnected]" << std::endl;
}
