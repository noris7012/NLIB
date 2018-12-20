#include "NClientHandler.h"

#include <iostream>

#include "GamePacket.h"

void NClientHandler::HandleConnected(PGameClient session)
{
	std::cout << "[Client Connected]" << std::endl;
}

void NClientHandler::HandlePacket(PGameClient session, PGamePacket packet)
{
	packet->Print();
}

void NClientHandler::HandleDisconnected(PGameClient session)
{
	std::cout << "[Client Disconnected]" << std::endl;
}
