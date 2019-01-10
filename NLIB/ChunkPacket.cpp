#include "ChunkPacket.h"

ChunkPacket* ChunkPacket::Deserialize(ByteStream& stream)
{
	E_PACKET_ID packet_id;
	if (!stream.Read<E_PACKET_ID>(packet_id)) return nullptr;

	ChunkPacket* packet = nullptr;
	switch (packet_id)
	{
	case E_PACKET_ID::CHUNK_NONE:
		packet = new ChunkPacketNone();
		break;
	case E_PACKET_ID::CHUNK_SOME:
		packet = new ChunkPacketSome();
		break;
	default:
		return nullptr;
	}

	auto rc = packet->Read(stream);
	if (rc != E_READ_RESULT::SUCCESS)
		return nullptr;

	return packet;
}

ChunkPacket::ChunkPacket()
{
}

void ChunkPacketNone::WriteHeader(ByteArrayPtr data)
{
	data->Set(NLIB_OFFSET_CHUNK, GetID());
}

E_READ_RESULT ChunkPacketNone::Read(ByteStream& stream)
{
	_data = stream.GetData();

	return E_READ_RESULT::SUCCESS;
}

void ChunkPacketSome::WriteHeader(ByteArrayPtr data)
{
	data->Set(NLIB_OFFSET_CHUNK + 0, GetID());
	data->Set(NLIB_OFFSET_CHUNK + 1, _chunk_id);
	data->Set(NLIB_OFFSET_CHUNK + 3, _slice_length);
	data->Set(NLIB_OFFSET_CHUNK + 5, _slice_id);
}

E_READ_RESULT ChunkPacketSome::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_chunk_id, uint16_t);
	NLIB_STREAM_READ(_slice_length, uint16_t);
	NLIB_STREAM_READ(_slice_id, uint16_t);

	_payload_offset = stream.Index();
	_data = stream.GetData();

	return E_READ_RESULT::SUCCESS;
}
