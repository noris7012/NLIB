#ifndef NLIB_GAME_SESSION_H
#define NLIB_GAME_SESSION_H

#include <memory>

#include "GameServerHandler.h"
#include "NetworkStruct.h"
#include "NetworkSession.h"
#include "ReliableSession.h"

class NetworkSession;
class ReliableSession;

class GameSession : public std::enable_shared_from_this<GameSession>
{
public:
	GameSession(PGameServerHandler handler, NetworkSession* network_session);
	~GameSession();

	void OnRecv(ProtocolPacket* recv);
	void OnRecvT(NLIBData data);

	void OnSend(NLIBData data);

private:
	PGameServerHandler _handler;
	NetworkSession* _network_session;
	ReliableSession* _reliable_session;

	BufferPool _buffer_pool;
};

using PGameSession = std::shared_ptr<GameSession>;

#endif
