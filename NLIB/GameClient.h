#ifndef NLIB_GAME_CLIENT_H
#define NLIB_GAME_CLIENT_H

#include "NetworkDefine.h"
#include "NetworkClient.h"
#include "ReliableLayer.h"

class GameClient : public std::enable_shared_from_this<GameClient>, public NetworkLayer, public GameEndpoint
{
public:
	GameClient(PGameClientHandler handler);

	bool Connect(GameConfig& config);
	bool IsConnected();
	void Disconnect();

	void Update(uint64_t time) override;
	void Read(UNLIBData data) override;
	void Write(UNLIBData data) override;
	void WritePacket(const byte* bytes, uint32_t length);

private:
	PGameClientHandler _handler;

	NetworkClient* _network_client;
	ReliableLayer* _reliable_layer;
};

#endif