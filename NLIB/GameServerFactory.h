#ifndef NLIB_GAME_SERVER_FACTORY_H
#define NLIB_GAME_SERVER_FACTORY_H
#include "GameServerInterface.h"
#include "GameServerHandler.h"

class GameServerFactory
{
public:
	static GameServerInterface * Create(GameServerHandler* handler);
};

#endif