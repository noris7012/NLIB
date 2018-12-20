#include "NServerHandler.h"

#include <iostream>

#include "GamePacket.h"
#include "GameSession.h"

void NServerHandler::HandleConnected(PGameSession session)
{
	std::cout << "[Session Connected]" << std::endl;
}

void NServerHandler::HandlePacket(PGameSession session, PGamePacket packet)
{
	packet->Print();

	session->WritePacket(reinterpret_cast<const byte*>("test"), 5);
}

void NServerHandler::HandleDisconnected(PGameSession session)
{
	std::cout << "[Session Disconnected]" << std::endl;
}
