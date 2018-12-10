#ifndef NLIB_NETWORK_ENDPOINT_H
#define NLIB_NETWORK_ENDPOINT_H

#include <thread>
#include <queue>
#include <mutex>

#include "NetworkStruct.h"
#include "NetworkConfig.h"
#include "TransportLayer.h"

class TransportLayer;

class NetworkEndpoint
{
public:
	NetworkEndpoint();
	~NetworkEndpoint();

	void Startup(NetworkConfig& config);
	bool IsConnected();
	void HandleReceive(S_Recv_Ptr data);
	void Send(const char* data);

private:
	void InternalUpdate(long time);

public:
	virtual void Update(long time) = 0;

private:
	std::thread* _thread;
	bool _running;

	TransportLayer* _transport;

	std::mutex _recv_queue_mutex;
	std::queue<S_Recv_Ptr> _recv_queue;
};

#endif