#include "ChunkLayer.h"

#include "ByteStream.h"

ChunkLayer::ChunkLayer(GameEndpoint* endpoint)
	: _endpoint(endpoint)
{
}

void ChunkLayer::Read(ReadParam& param)
{
	auto data = param.data;
	auto offset = param.offset;

	ByteStream stream(data, offset);
	ChunkPacket* packet = ChunkPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	if (packet->GetID() == E_PACKET_ID::CHUNK_NONE)
	{
		auto p = static_cast<ChunkPacketNone*>(packet);

		param.offset += NLIB_HEADER_SIZE_CHUNK_NONE;
		ReadNext(param);
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
			ReadParam new_param = { holder->GetData(), 0 };
			ReadNext(new_param);
		}
	}
}

void ChunkLayer::Write(const WriteParam& param)
{
	auto data = param.data;

	if (data->Length() <= NLIB_SLICE_MAX_SIZE)
	{
		auto new_data = std::make_shared<ByteArray>(MAX_MTU_SIZE);

		ChunkPacketNone packet;
		packet.WriteHeader(new_data);

		new_data->Set(NLIB_OFFSET_PAYLOAD, data, 0);

		WriteNext(WriteParam{ new_data, NLIB_CHUNK_RETRY_LIMIT_NONE });
	}
	else
	{
		auto holder = new ChunkHolder();
		holder->Set(_chunk_id++);

		SetSendBuffer(holder->GetChunkId(), holder);

		auto cnt = holder->Split(data);
		for (uint32_t i = 0; i < cnt; ++i)
		{
			auto packet = holder->GetPacket(i);

			assert(packet != nullptr);
			if (packet == nullptr)
				continue;

			WriteNext(WriteParam{ packet->GetData(), NLIB_CHUNK_RETRY_LIMIT_SOME });
		}
	}
}

void ChunkLayer::Fail(const FailParam& param)
{
	auto data = param.data;

	ByteStream stream(data, NLIB_OFFSET_CHUNK);
	ChunkPacket* packet = ChunkPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	if (packet->GetID() == E_PACKET_ID::CHUNK_NONE)
	{
		auto p = static_cast<ChunkPacketNone*>(packet);

		FailNext(FailParam{ p->GetData() });
	}
	else
	{
		auto p = static_cast<ChunkPacketSome*>(packet);
		auto holder = GetSendBuffer(p->GetChunkId());

		assert(holder != nullptr);
		FailNext(FailParam{ holder->GetData() });
	}
}

void ChunkLayer::Update(uint64_t time)
{
	for (uint32_t i = 0; i < NLIB_CHUNK_BUFFER_SIZE; ++i)
	{
		auto buffer = _send_buffer[i];
		if (buffer == nullptr || buffer->IsAcked())
			continue;

		// TODO acked 贸府

		if (buffer->IsTimeout(_endpoint->GetRTT()))
		{
			_send_buffer[i] = nullptr;
			FailNext(FailParam{ buffer->GetData() });
		}
	}

	for (uint32_t i = 0; i < NLIB_CHUNK_BUFFER_SIZE; ++i)
	{
		auto buffer = _recv_buffer[i];
		if (buffer == nullptr || buffer->IsReadCompleted())
			continue;

		// TODO Completed 贸府

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
		FailNext(FailParam{ old_packet->GetData() });
	}

	_recv_buffer[chunk_id % NLIB_CHUNK_BUFFER_SIZE] = holder;
}
