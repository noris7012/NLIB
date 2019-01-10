#ifndef NLIB_CHUNK_LAYER_H
#define NLIB_CHUNK_LAYER_H

#include <cstdint>

#include "NetworkLayer.h"
#include "ChunkHolder.h"
#include "GameEndpoint.h"

class ChunkLayer : public NetworkLayer
{
public:
	ChunkLayer(GameEndpoint* endpoint);

public:
	void Read(ReadParam& param) override;
	void Write(const WriteParam& param) override;
	void Fail(const FailParam& param) override;
	void Update(uint64_t time);

	ChunkHolder* GetRecvBuffer(uint16_t chunk_id);
	void SetRecvBuffer(uint16_t chunk_id, ChunkHolder* holder);

	ChunkHolder* GetSendBuffer(uint16_t chunk_id);
	void SetSendBuffer(uint16_t chunk_id, ChunkHolder* holder);

private:
	GameEndpoint * _endpoint;

	uint16_t _chunk_id;

	ChunkHolder* _send_buffer[NLIB_CHUNK_BUFFER_SIZE];
	ChunkHolder* _recv_buffer[NLIB_CHUNK_BUFFER_SIZE];
};

#endif