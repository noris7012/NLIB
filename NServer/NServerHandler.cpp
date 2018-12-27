#include "NServerHandler.h"

#include <iostream>

#include "GamePacket.h"
#include "GameSession.h"
#include "Utility.h"
#include "Logger.h"

void NServerHandler::HandleConnected(PGameSession session)
{
	std::cout << "[Session Connected]" << std::endl;
}

void NServerHandler::HandlePacket(PGameSession session, PGamePacket packet)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[HandlePacket] " << Utility::TimeInHHMMSSMMM() << std::endl;

	packet->Print();
#endif

	session->WritePacket(reinterpret_cast<const byte*>("test"), 5);

	Logger::GetInstance()->Log(packet->GetString());
}

void NServerHandler::HandleDisconnected(PGameSession session)
{
	std::cout << "[Session Disconnected]" << std::endl;
}
