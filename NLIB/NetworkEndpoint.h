#ifndef NLIB_NETWORK_ENDPOINT_H
#define NLIB_NETWORK_ENDPOINT_H

#include <thread>

#include "NetworkConfig.h"

class NetworkEndpoint
{
public:
	NetworkEndpoint();
	~NetworkEndpoint();

	void startup(NetworkConfig& config);

public:
	void update(long time);

private:
	std::thread *_thread;
};

#endif