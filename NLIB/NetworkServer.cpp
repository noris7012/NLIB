#include "NetworkServer.h"

#include <iostream>
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

void NetworkServer::ProcessReceive(NLIBRecv* recv)
{
	ByteStream stream(recv->buffer->data, recv->length);
	ProtocolPacket* packet = ProtocolPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet != nullptr)
	{
		packet->Print();

		// TODO Test Code
		delete packet;
	}
}