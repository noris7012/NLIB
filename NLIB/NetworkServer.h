#ifndef NLIB_NETWORK_SERVER_H
#define NLIB_NETWORK_SERVER_H

#include <stdint.h>
#include <unordered_map>
#include <stdint.h>
#include <mutex>

#include "NetworkEndpoint.h"
#include "NetworkSession.h"
#include "ProtocolPacket.h"

class NetworkSession;

class NetworkServer : public NetworkEndpoint
{
public:
	NetworkServer();
	~NetworkServer();

	bool Listen(uint32_t port);

public:
	void Update(uint64_t time);
	void ProcessReceive(NLIBRecv* data);

private:
	void HandleConnectionRequest(ProtocolPacket*, NLIBRecv*);
	void HandleConnectionResponse(ProtocolPacket*, NLIBRecv*);
	void HandleConnectionKeepAlive(ProtocolPacket*, NLIBRecv*);
	void HandleConnectionPayload(ProtocolPacket*, NLIBRecv*);
	void HandleConnectionDisconnect(ProtocolPacket*, NLIBRecv*);

public:
	void OnConnected(NetworkSession* session);
	void OnDisconnected(NetworkSession* session);

private:
	std::mutex _connected_session_mutex;
	NetworkSession* _connected_session[NLIB_MAX_SESSION];
	std::unordered_map<uint64_t, NetworkSession*> _connected_session_by_address_id;
	std::unordered_map<uint64_t, NetworkSession*> _connected_session_by_id;

	uint64_t _challenge_token_sequence;
	NetworkSession* _connection_slot[NLIB_MAX_CONNECTION_SLOT];
};

#endif
