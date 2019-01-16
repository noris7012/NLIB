#ifndef NLIB_GAME_CLIENT_FACTORY_H
#define NLIB_GAME_CLIENT_FACTORY_H
#include "GameClientInterface.h"
#include "GameClientHandler.h"

class GameClientFactory
{
public:
	static GameClientInterface* Create(GameClientHandler* handler);
};

#endif
