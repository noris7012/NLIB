#include "NetworkServer.h"

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
	config.host = nullptr;
	config.port = port;

	Startup(config);

	return true;
}

void NetworkServer::Update(long time)
{

}