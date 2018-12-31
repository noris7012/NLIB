#ifndef NLIB_CHUNK_PACKET_H
#define NLIB_CHUNK_PACKET_H

#include <cstdint>

#include "NetworkDefine.h"
#include "NetworkStruct.h"
#include "ByteStream.h"

class ByteStream;

class ChunkPacket
{
public:
	static ChunkPacket* Deserialize(ByteStream& stream);

public:
	ChunkPacket();

	virtual E_PACKET_ID GetID() = 0;

private:

	// Read
	const byte* _data;
	uint32_t _data_length;

	// Write
	PNLIBData _send_data;
};

class ChunkPacketNone : public ChunkPacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::CHUNK_NONE; }

	PNLIBData GetData();
};

class ChunkPacketSome : public ChunkPacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::CHUNK_SOME; }

	uint32_t GetChunkId() { return _chunk_id; }

private:
	uint16_t _chunk_id;
	uint16_t _slice_length;
	uint16_t _slice_id;
	bool _acked;
};

#endif