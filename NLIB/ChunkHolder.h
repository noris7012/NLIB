#ifndef NLIB_CHUNK_HOLDER_H
#define NLIB_CHUNK_HOLDER_H

#include "ChunkPacket.h"

class ChunkHolder
{
public:

private:
	uint16_t _chunk_id;

	ChunkPacket* _buffer[NLIB_CHUNK_BUFFER_SIZE];
};

#endif