#include "NServerHandler.h"

#include <iostream>

#include "GamePacket.h"
#include "GameSession.h"
#include "Utility.h"
#include "Logger.h"

void NServerHandler::HandleConnected(GameSessionInterface* session)
{
	std::cout << "[Session Connected]" << std::endl;
}

void NServerHandler::HandlePacket(GameSessionInterface* session, GamePacket packet)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[HandlePacket] " << Utility::TimeInHHMMSSMMM() << std::endl;

	packet->Print();
#endif

	session->WritePacket(reinterpret_cast<const byte*>("test"), 5);

	std::string str("data07");

	std::stringstream ss;
	ss << "[Comp] " << str << " , " << packet.GetString() << " = " << str.compare(packet.GetString());

	Logger::GetInstance()->Log(ss.str());

	if (str.compare(packet.GetString()) == 0)
	{
		Logger::GetInstance()->Log("Quit");
		session->Close();
		return;
	}

	Logger::GetInstance()->Log(packet.GetString());
}

void NServerHandler::HandleDisconnected(GameSessionInterface* session)
{
	std::cout << "[Session Disconnected]" << std::endl;
}
