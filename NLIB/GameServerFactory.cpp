#include "GameServerFactory.h"
#include "GameServer.h"

GameServerInterface* GameServerFactory::Create(GameServerHandler* handler)
{
	return new GameServer(handler);
}
