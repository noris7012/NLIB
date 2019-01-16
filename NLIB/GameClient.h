#ifndef NLIB_GAME_CLIENT_H
#define NLIB_GAME_CLIENT_H

#include "NetworkDefine.h"
#include "NetworkClient.h"
#include "ReliableLayer.h"
#include "GameClientInterface.h"

class GameClient : public NetworkLayer, public GameEndpoint, public GameClientInterface
{
public:
	GameClient(GameClientHandler* handler);

	bool Connect(GameConfig& config) override;
	bool IsConnected() override;
	void Disconnect() override;

	void Update(uint64_t time) override;
	void Read(ReadParam& data) override;
	void WritePacket(char* bytes, int length) override;
	void Fail(const FailParam& param) override;

private:
	GameClientHandler* _handler;

	NetworkClient* _network_client;
	ReliableLayer* _reliable_layer;
	ChunkLayer* _chunk_layer;
};

#endif