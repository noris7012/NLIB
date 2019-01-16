#include "GameClientFactory.h"

#include <memory>

#include "GameClient.h"

GameClientInterface* GameClientFactory::Create(GameClientHandler* handler)
{
	return new GameClient(handler);
}
