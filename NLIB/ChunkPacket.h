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
	virtual NLIBData GetHeader() = 0;
	virtual E_READ_RESULT Read(ByteStream& stream) = 0;

protected:

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
	NLIBData GetHeader() override;
	E_READ_RESULT Read(ByteStream& stream) override;

	PNLIBData GetData();
};

class ChunkPacketSome : public ChunkPacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::CHUNK_SOME; }
	NLIBData GetHeader() override;
	E_READ_RESULT Read(ByteStream& stream) override;

	uint16_t GetChunkId() { return _chunk_id; }
	PNLIBData GetSendData();
	void SetSendData(PNLIBData send_data);

	uint16_t GetSliceId() { return _slice_id; }
	uint16_t GetSliceLength() { return _slice_length; }

	uint32_t GetDataLength() { return _data == nullptr ? _send_data->length : _data_length; }
	const byte* GetData() { return _data == nullptr ? _send_data->bytes : _data; }

private:
	uint16_t _chunk_id;
	uint16_t _slice_length;
	uint16_t _slice_id;
	bool _acked;
};

#endif