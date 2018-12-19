#ifndef NLIB_NETWORK_SERVER_H
#define NLIB_NETWORK_SERVER_H

#include <cstdint>
#include <unordered_map>
#include <cstdint>
#include <mutex>

#include "NetworkEndpoint.h"
#include "NetworkSession.h"
#include "ProtocolPacket.h"
#include "CUDPLayer.h"

class NetworkSession;

class GameServer final : public CUDPLayer
{
public:
	bool Listen(uint32_t port);

public:
	void Update(uint64_t time) override;
	void Send(NLIBAddress& address, ProtocolPacket& packet);
	void OnRecv(NLIBRecv* recv) override;

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
