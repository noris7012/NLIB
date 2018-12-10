#ifndef NLIB_NETWORK_SERVER_H
#define NLIB_NETWORK_SERVER_H

#include <stdint.h>

#include "NetworkEndpoint.h"

class NetworkServer : public NetworkEndpoint
{
public:
	NetworkServer();
	~NetworkServer();

	bool Listen(uint32_t port);

public:
	void Update(long time);

private:


};

#endif
