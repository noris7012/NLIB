#include "ByteStream.h"

#include <cassert>
#include <cstring>

#include <iostream>

ByteStream::ByteStream(const ByteArrayPtr& data, uint32_t offset)
{
	_data = data;
	_capacity = data->Capacity();
	_idx = offset;
}

void ByteStream::Write(E_PACKET_ID packet_id)
{
	assert(_idx + 1 <= _capacity);

	if (_idx + 1 <= _capacity)
	{
		_data->Set(_idx++, packet_id);
	}
}

void ByteStream::Write(uint32_t v)
{
	assert(_idx + 4 <= _capacity);

	if (_idx + 4 <= _capacity)
	{
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 0));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 1));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 2));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 3));
	}
}

void ByteStream::Write(uint64_t v)
{
	assert(_idx + 8 <= _capacity);

	if (_idx + 8 <= _capacity)
	{
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 0));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 1));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 2));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 3));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 4));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 5));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 6));
		_data->Set(_idx++, static_cast<byte>(v >> 8 * 7));
	}
}

void ByteStream::Write(const byte* data, uint32_t length)
{
	assert(_idx + length <= _capacity);

	if (_idx + length <= _capacity)
	{
		_data->Set(_idx, data, length);
		_idx += length;
	}
}

void ByteStream::PadWithZero()
{
	if (_capacity - _idx > 0)
	{
		_data->SetZero(_idx, _capacity - _idx);
		_idx = _capacity;
	}
}

uint32_t ByteStream::Length()
{
	return _idx;
}

uint32_t ByteStream::Remain()
{
	return _capacity - _idx;
}