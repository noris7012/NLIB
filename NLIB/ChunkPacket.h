#ifndef NLIB_CHUNK_PACKET_H
#define NLIB_CHUNK_PACKET_H

#include <cstdint>

#include "NetworkDefine.h"
#include "NetworkStruct.h"
#include "ByteStream.h"
#include "ByteArray.h"

class ByteStream;

class ChunkPacket
{
public:
	static ChunkPacket* Deserialize(ByteStream& stream);

public:
	ChunkPacket();

	virtual E_PACKET_ID GetID() = 0;
	virtual void WriteHeader(ByteArrayPtr data) = 0;
	virtual E_READ_RESULT Read(ByteStream& stream) = 0;
};

class ChunkPacketNone : public ChunkPacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::CHUNK_NONE; }
	void WriteHeader(ByteArrayPtr data) override;
	E_READ_RESULT Read(ByteStream& stream) override;

	ByteArrayPtr GetData() { return _data; }

private:
	ByteArrayPtr _data;
};

class ChunkPacketSome : public ChunkPacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::CHUNK_SOME; }
	void WriteHeader(ByteArrayPtr data) override;
	E_READ_RESULT Read(ByteStream& stream) override;

	uint16_t GetChunkId() { return _chunk_id; }

	void Init(uint16_t chunk_id, uint16_t slice_length, uint16_t slice_id)
	{
		_chunk_id = chunk_id;
		_slice_length = slice_length;
		_slice_id = slice_id;
	}

	void SetData(ByteArrayPtr data)
	{
		_data = data;
	}

	uint16_t GetSliceId() { return _slice_id; }
	uint16_t GetSliceLength() { return _slice_length; }

	ByteArrayPtr GetData() { return _data; }
	uint32_t GetDataLength() { return _data->Length(); }
	uint32_t GetPayloadOffset() { return _payload_offset; }

private:
	ByteArrayPtr _data;
	uint16_t _chunk_id;
	uint16_t _slice_length;
	uint16_t _slice_id;

	bool _acked;
	uint32_t _payload_offset;
};

#endif