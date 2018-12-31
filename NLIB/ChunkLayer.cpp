#include "ChunkLayer.h"

#include "ByteStream.h"

ChunkLayer::ChunkLayer()
{
}

void ChunkLayer::Read(PNLIBData data)
{
	ByteStream stream(const_cast<byte*>(data->bytes), data->length);
	ChunkPacket* packet = ChunkPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	if (packet->GetID() == E_PACKET_ID::CHUNK_NONE)
	{
		auto p = static_cast<ChunkPacketNone*>(packet);

		ReadNext(p->GetData());
	}
	else
	{
		auto p = static_cast<ChunkPacketSome*>(packet);
		auto holder = GetChunkHolder(p->GetChunkId());

		// TODO °è¼Ó..
	}
}

void ChunkLayer::Write(PNLIBData data)
{
}
