#ifndef NLIB_NETWORK_SERVER_H
#define NLIB_NETWORK_SERVER_H

#include <cstdint>
#include <unordered_map>
#include <cstdint>
#include <mutex>

#include "NetworkDefine.h"
#include "NetworkEndpoint.h"
#include "NetworkSession.h"
#include "ProtocolPacket.h"
#include "GameConfig.h"
#include "GameSession.h"

class NetworkSession;
class GameSession;

class GameServer : public NetworkEndpoint
{
public:
	GameServer(PGameServerHandler handler);

public:
	bool Listen(GameConfig& config);

public:
	void Update(uint64_t time) override;
	void Send(NLIBAddress& address, ProtocolPacket& packet);
	void Send(NLIBAddress& address, UNLIBData data);
	void HandleReceive(NLIBRecv* recv) override;

private:
	void HandleConnectionRequest(ProtocolPacket*, NLIBRecv*);
	void HandleConnectionResponse(ProtocolPacket*, NLIBRecv*);
	//void HandleConnectionKeepAlive(ProtocolPacket*, NLIBRecv*);
	//void HandleConnectionPayload(ProtocolPacket*, NLIBRecv*);
	//void HandleConnectionDisconnect(ProtocolPacket*, NLIBRecv*);

public:
	void OnConnected(NetworkSession* session);
	void OnDisconnected(NetworkSession* session);

private:
	PGameServerHandler _handler;
	BufferPool _buffer_pool;

	std::mutex _connected_session_mutex;
	PGameSession _connected_session[NLIB_MAX_SESSION] = {};
	std::unordered_map<uint64_t, PGameSession> _connected_session_by_address_id;
	std::unordered_map<uint64_t, PGameSession> _connected_session_by_id;

	uint64_t _challenge_token_sequence = 0;
	NetworkSession* _connection_slot[NLIB_MAX_CONNECTION_SLOT] = {};
	
};

#endif
