#ifndef NLIB_CHUNK_LAYER_H
#define NLIB_CHUNK_LAYER_H

#include <cstdint>

#include "NetworkLayer.h"
#include "ChunkHolder.h"

class ChunkLayer : public NetworkLayer
{
public:
	ChunkLayer();

public:
	void Read(PNLIBData data) override;
	void Write(PNLIBData data) override;

	ChunkHolder* GetChunkHolder(uint32_t chunk_id);

private:
	uint16_t _chunk_id;

	ChunkHolder* _send_buffer[NLIB_CHUNK_BUFFER_SIZE];
	ChunkHolder* _recv_buffer[NLIB_CHUNK_BUFFER_SIZE];
};

#endif