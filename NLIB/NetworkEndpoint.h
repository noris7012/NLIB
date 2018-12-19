#ifndef NLIB_NETWORK_ENDPOINT_H
#define NLIB_NETWORK_ENDPOINT_H

#include <thread>
#include <queue>
#include <mutex>
#include <cstdint>

#include "NetworkStruct.h"
#include "NetworkConfig.h"
#include "TransportLayer.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"
#include "BufferPool.h"

class TransportLayer;
class CUDPLayer;

class NetworkEndpoint
{
public:
	NetworkEndpoint();

	void Startup(NetworkConfig& config);
	void Destroy();
	void HandleReceive(char* data, std::size_t length, NLIBAddress& address);
	void Send(NLIBAddress& address, const byte* data, uint32_t length);

private:
	void InternalUpdate(uint64_t time);

private:
	NLIBRecv* Pop();

public:
	virtual void Update(uint64_t time) = 0;
	virtual void OnRecv(NLIBRecv* data) = 0;

private:
	std::thread* _thread;
	bool _running;

	TransportLayer* _transport;

	std::mutex _recv_queue_mutex;
	std::queue<NLIBRecv*> _recv_queue;

	BufferPool _buffer_pool;
};

#endif