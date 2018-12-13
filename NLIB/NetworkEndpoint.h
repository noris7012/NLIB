#ifndef NLIB_NETWORK_ENDPOINT_H
#define NLIB_NETWORK_ENDPOINT_H

#include <thread>
#include <queue>
#include <mutex>

#include "NetworkStruct.h"
#include "NetworkConfig.h"
#include "TransportLayer.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"
#include "BufferPool.h"

class TransportLayer;

class NetworkEndpoint
{
public:
	NetworkEndpoint();
	~NetworkEndpoint();

	void Startup(NetworkConfig& config);
	bool IsConnected();
	void HandleReceive(char* data, std::size_t length, NLIBAddress& address);
	void Send(ProtocolPacket& packet);

private:
	void InternalUpdate(long time);

private:
	NLIBRecv* Pop();

public:
	virtual void Update(long time) = 0;
	virtual void ProcessReceive(NLIBRecv* data) = 0;

private:
	std::thread* _thread;
	bool _running;

	TransportLayer* _transport;

	std::mutex _recv_queue_mutex;
	std::queue<NLIBRecv*> _recv_queue;

	BufferPool _buffer_pool;
};

#endif