#ifndef NLIB_NETWORK_CLIENT_H
#define NLIB_NETWORK_CLIENT_H

#include <map>
#include <vector>

#include "NetworkEndpoint.h"
#include "ClientState.h"

class NetworkClient : public NetworkEndpoint
{
public:
	NetworkClient();
	~NetworkClient();

	bool connect(const char* host, unsigned short port);

public:
	void Update(long time);

private:
	bool SetState(E_CLIENT_STATE_ID state_id);

	void STATE_CREATE(E_CLIENT_STATE_ID state_id);
	void TRANSITION_CREATE(E_CLIENT_STATE_ID state_id, E_CLIENT_STATE_ID transition_list[]);

private:
	ClientState* _state;
	std::map<E_CLIENT_STATE_ID, ClientState*> _state_map;
	std::map<E_CLIENT_STATE_ID, std::vector<E_CLIENT_STATE_ID>*> _state_transition_table;
};

#endif