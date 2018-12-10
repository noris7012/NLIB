#include "NetworkServer.h"

#include <assert.h>

NetworkServer::NetworkServer()
{

}

NetworkServer::~NetworkServer()
{

}

bool NetworkServer::Listen(uint32_t port)
{
	// TODO Check State
	NetworkConfig config;
	config.transport_type = UDP;
	config.port = port;

	assert(config.host == nullptr);

	Startup(config);

	return true;
}

void NetworkServer::Update(long time)
{

}