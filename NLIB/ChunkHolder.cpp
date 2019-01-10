#include "ChunkHolder.h"
#include "Utility.h"

ChunkHolder::ChunkHolder()
	: _chunk_id(0)
	  , _slice_length(0)
	  , _read_slice_count(0)
	  , _acked(false)
	  , _buffer{}
{
	_create_time = Utility::GetTime();
}

void ChunkHolder::Set(uint16_t chunk_id)
{
	_chunk_id = chunk_id;
}

void ChunkHolder::Read(ChunkPacketSome* packet)
{
	auto slice_id = packet->GetSliceId();

	// TODO 肋给等 菩哦 贸府
	assert(slice_id >= 0 && slice_id < NLIB_CHUNK_BUFFER_SIZE);
	if (!(slice_id >= 0 && slice_id < NLIB_CHUNK_BUFFER_SIZE))
		return;

	if (_buffer[slice_id] != nullptr)
		return;

	_buffer[slice_id] = packet;
	++_read_slice_count;
	_slice_length = packet->GetSliceLength();
}

bool ChunkHolder::IsReadCompleted()
{
	return _slice_length > 0 && _read_slice_count == _slice_length;
}

uint32_t ChunkHolder::Split(const ByteArrayPtr& data)
{
	auto cnt = static_cast<uint16_t>(data->Length() % NLIB_SLICE_MAX_SIZE == 0 ? data->Length() / NLIB_SLICE_MAX_SIZE : data->Length() / NLIB_SLICE_MAX_SIZE + 1);

	for (uint16_t i = 0; i < cnt; ++i)
	{
		auto new_data = std::make_shared<ByteArray>(MAX_MTU_SIZE);

		auto buffer = new ChunkPacketSome();
		buffer->Init(_chunk_id, cnt, i);

		buffer->SetData(new_data);

		buffer->WriteHeader(new_data);

		auto length = i == cnt - 1 ? data->Length() % NLIB_SLICE_MAX_SIZE : NLIB_SLICE_MAX_SIZE;
		new_data->Set(NLIB_OFFSET_PAYLOAD_CHUNK, data->Bytes() + i * NLIB_SLICE_MAX_SIZE, length);
		
		_buffer[i] = buffer;
	}

	_slice_length = cnt;

	return cnt;
}

ByteArrayPtr ChunkHolder::GetData()
{
	if (_slice_length == 0)
		return nullptr;

	uint32_t length = 0;
	for (uint32_t i = 0; i < _slice_length; ++i)
	{
		if (_buffer[i] == nullptr)
			return nullptr;

		if (i == _slice_length - 1)
		{
			length += _buffer[i]->GetDataLength();
		}
		else
		{
			length += NLIB_SLICE_MAX_SIZE;
		}
	}

	auto data = std::make_shared<ByteArray>(length);
	for (uint32_t i = 0; i < _slice_length; ++i)
	{
		auto buffer = _buffer[i];
		data->Set(i * NLIB_SLICE_MAX_SIZE, buffer->GetData(), buffer->GetPayloadOffset());
	}

	return data;
}

ChunkPacketSome* ChunkHolder::GetPacket(uint32_t slice_id)
{
	assert(slice_id >= 0 && slice_id < NLIB_CHUNK_BUFFER_SIZE);
	if (!(slice_id >= 0 && slice_id < NLIB_CHUNK_BUFFER_SIZE))
		return nullptr;

	return _buffer[slice_id];
}

bool ChunkHolder::IsTimeout(float rtt)
{
	auto now = Utility::GetTime();

	return rtt > 0 && now >= _create_time + uint64_t(6 * rtt * 1000);
}
