#ifndef NLIB_GAME_SESSION_H
#define NLIB_GAME_SESSION_H

#include <memory>

#include "GameEndpoint.h"
#include "GameServerHandler.h"
#include "NetworkStruct.h"
#include "NetworkSession.h"
#include "ReliableLayer.h"
#include "NetworkLayer.h"

class GameServerHandler;
class NetworkSession;
class ReliableLayer;

class GameSession : public std::enable_shared_from_this<GameSession>, public NetworkLayer, public GameEndpoint
{
public:
	GameSession(std::shared_ptr<GameServerHandler> handler, NetworkSession* network_session);
	~GameSession();

	void RecvPacket(ProtocolPacket* recv);
	void WritePacket(const byte* bytes, uint32_t length);

	void Read(UNLIBData data) override;
	void Write(UNLIBData data) override;
	void Update(uint64_t time) override;

private:
	std::shared_ptr<GameServerHandler> _handler;
	NetworkSession* _network_session;
	ReliableLayer* _reliable_session;

	BufferPool _buffer_pool;
};

#endif
