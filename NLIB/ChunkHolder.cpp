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

	// TODO 잘못된 패킷 처리
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

uint32_t ChunkHolder::Split(PNLIBData data)
{
	// TODO data 는 recursive함.
	auto cnt = data->length % NLIB_SLICE_MAX_SIZE == 0 ? data->length / NLIB_SLICE_MAX_SIZE : data->length / NLIB_SLICE_MAX_SIZE + 1;

	for (uint32_t i = 0; i < cnt; ++i)
	{
		auto buffer = new ChunkPacketSome();

		auto length = i == cnt - 1 ? data->length % NLIB_SLICE_MAX_SIZE : NLIB_SLICE_MAX_SIZE;
		auto bytes = new byte[length];
		memcpy_s(bytes, length, data->bytes + i * NLIB_SLICE_MAX_SIZE, length);

		auto new_data = NLIBData::Instance();
		new_data->bytes = const_cast<const byte*>(bytes);
		new_data->length = length;

		buffer->SetSendData(new_data);
		
		_buffer[i] = buffer;
	}

	_slice_length = cnt;

	return cnt;
}

PNLIBData ChunkHolder::GetData()
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

	auto bytes = new byte[length];
	for (uint32_t i = 0; i < _slice_length; ++i)
	{
		auto buffer = _buffer[i];
		memcpy_s(bytes + i * NLIB_SLICE_MAX_SIZE, length, buffer->GetData(), buffer->GetDataLength());
	}

	auto new_data = NLIBData::Instance();
	new_data->bytes = const_cast<const byte*>(bytes);
	new_data->length = length;

	return new_data;
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
