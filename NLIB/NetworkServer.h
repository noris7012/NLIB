#ifndef NLIB_NETWORK_SERVER_H
#define NLIB_NETWORK_SERVER_H

#include <stdint.h>
#include <unordered_map>
#include <stdint.h>

#include "NetworkEndpoint.h"
#include "NetworkSession.h"

class NetworkServer : public NetworkEndpoint
{
public:
	NetworkServer();
	~NetworkServer();

	bool Listen(uint32_t port);

public:
	void Update(long time);
	void ProcessReceive(NLIBRecv* data);

private:
	std::unordered_map<uint64_t, NetworkSession*> _connected_session_by_address_id;
	std::unordered_map<uint64_t, NetworkSession*> _connected_session_by_id;

};

#endif
