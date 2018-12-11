#ifndef NLIB_NETWORK_CLIENT_H
#define NLIB_NETWORK_CLIENT_H

#include <map>
#include <vector>

#include "NetworkEndpoint.h"
#include "ClientState.h"
#include "ProtocolPacket.h"

class ClientState;

class NetworkClient : public NetworkEndpoint
{
public:
	NetworkClient();
	~NetworkClient();

	bool connect(const char* host, unsigned short port);

public:
	void Update(long time);
	void ProcessReceive(NLIBRecv* data);

public:
	bool SetState(E_CLIENT_STATE_ID state_id);

private:
	ClientState* _state;
	std::map<E_CLIENT_STATE_ID, ClientState*> _state_map;
	std::map<E_CLIENT_STATE_ID, std::vector<E_CLIENT_STATE_ID>*> _state_transition_table;
};

#endif