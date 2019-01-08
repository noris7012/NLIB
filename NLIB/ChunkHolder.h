#ifndef NLIB_CHUNK_HOLDER_H
#define NLIB_CHUNK_HOLDER_H

#include "ChunkPacket.h"

class ChunkHolder
{
public:
	ChunkHolder();

public:
	void Set(uint16_t chunk_id);

	void Read(ChunkPacketSome* packet);
	bool IsReadCompleted();

	uint32_t Split(const ByteArrayPtr& data);
	ByteArrayPtr GetData();

	ChunkPacketSome* GetPacket(uint32_t slice_id);

	uint16_t GetChunkId() { return _chunk_id; }

	bool IsAcked() { return _acked; }
	bool IsTimeout(float rtt);
private:
	uint16_t _chunk_id;
	uint16_t _slice_length;

	uint16_t _read_slice_count;

	bool _acked;
	uint64_t _create_time;

	ChunkPacketSome* _buffer[NLIB_CHUNK_BUFFER_SIZE];
};

#endif