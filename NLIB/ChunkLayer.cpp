#include "ChunkLayer.h"

#include "ByteStream.h"

ChunkLayer::ChunkLayer(GameEndpoint* endpoint)
	: _endpoint(endpoint)
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
		auto holder = GetRecvBuffer(p->GetChunkId());

		if (holder == nullptr)
		{
			holder = new ChunkHolder();
			holder->Set(p->GetChunkId());

			SetRecvBuffer(holder->GetChunkId(), holder);
		}

		holder->Read(p);

		if (holder->IsReadCompleted())
		{
			ReadNext(holder->GetData());
		}
	}
}

void ChunkLayer::Write(PNLIBData data)
{
	// TODO length 는 전체 데이터 길이가 아님. Recursive한 구조이기 때문에.
	if (data->length <= NLIB_SLICE_MAX_SIZE)
	{
		ChunkPacketNone packet;
		auto header = packet.GetHeader();

		auto new_data = NLIBData::Instance();
		new_data->bytes = header.bytes;
		new_data->length = header.length;
		new_data->next = data;

		WriteNext(new_data);
	}
	else
	{
		auto holder = new ChunkHolder();
		SetSendBuffer(holder->GetChunkId(), holder);

		auto cnt = holder->Split(data);
		for (uint32_t i = 0; i < cnt; ++cnt)
		{
			auto packet = holder->GetPacket(i);

			assert(packet != nullptr);
			if (packet == nullptr)
				continue;

			auto header = packet->GetHeader();

			auto new_data = NLIBData::Instance();
			new_data->bytes = header.bytes;
			new_data->length = header.length;
			new_data->next = packet->GetSendData();

			WriteNext(new_data);
		}
	}
}

void ChunkLayer::Fail(PNLIBData data)
{
	ByteStream stream(const_cast<byte*>(data->bytes), data->length);
	ChunkPacket* packet = ChunkPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	if (packet->GetID() == E_PACKET_ID::CHUNK_NONE)
	{
		auto p = static_cast<ChunkPacketNone*>(packet);

		FailNext(p->GetData());
	}
	else
	{
		auto p = static_cast<ChunkPacketSome*>(packet);
		auto holder = GetSendBuffer(p->GetChunkId());

		assert(holder != nullptr);
		FailNext(holder->GetData());
	}
}

void ChunkLayer::Update(uint64_t time)
{
	for (uint32_t i = 0; i < NLIB_CHUNK_BUFFER_SIZE; ++i)
	{
		auto buffer = _send_buffer[i];
		if (buffer == nullptr || buffer->IsAcked())
			continue;

		// TODO acked 처리

		if (buffer->IsTimeout(_endpoint->GetRTT()))
		{
			_send_buffer[i] = nullptr;
			FailNext(buffer->GetData());
		}
	}

	for (uint32_t i = 0; i < NLIB_CHUNK_BUFFER_SIZE; ++i)
	{
		auto buffer = _recv_buffer[i];
		if (buffer == nullptr || buffer->IsReadCompleted())
			continue;

		// TODO Completed 처리

		if (buffer->IsTimeout(_endpoint->GetRTT()))
		{
			_recv_buffer[i] = nullptr;
			// TODO Destroy Buffer
		}
	}
}

ChunkHolder* ChunkLayer::GetRecvBuffer(uint16_t chunk_id)
{
	auto packet = _recv_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE];
	if (packet != nullptr && packet->GetChunkId() != chunk_id)
		return nullptr;

	return packet;
}

void ChunkLayer::SetRecvBuffer(uint16_t chunk_id, ChunkHolder* holder)
{
	auto old_packet = _recv_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE];
	if (old_packet != nullptr)
	{
		// TODO
		delete old_packet;
	}

	_recv_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE] = holder;
}

ChunkHolder* ChunkLayer::GetSendBuffer(uint16_t chunk_id)
{
	auto packet = _send_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE];
	if (packet != nullptr && packet->GetChunkId() != chunk_id)
		return nullptr;

	return packet;
}

void ChunkLayer::SetSendBuffer(uint16_t chunk_id, ChunkHolder* holder)
{
	auto old_packet = _send_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE];
	if (old_packet != nullptr)
	{
		// TODO
		FailNext(old_packet->GetData());
	}

	_recv_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE] = holder;
}
