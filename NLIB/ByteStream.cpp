#include "ByteStream.h"

#include <cassert>
#include <cstring>

#include <iostream>

ByteStream::ByteStream(const ByteArrayPtr& data, uint32_t offset)
{
	_data = data;
	_buffer = data->Bytes();
	_capacity = data->Capacity();
	_idx = offset;
}

void ByteStream::Write(E_PACKET_ID packet_id)
{
	assert(_idx + 1 <= _capacity);

	if (_idx + 1 <= _capacity)
	{
		_buffer[_idx++] = (unsigned char)packet_id;
	}
}

void ByteStream::Write(uint32_t v)
{
	assert(_idx + 4 <= _capacity);

	if (_idx + 4 <= _capacity)
	{
		_buffer[_idx++] = v >> 8 * 0;
		_buffer[_idx++] = v >> 8 * 1;
		_buffer[_idx++] = v >> 8 * 2;
		_buffer[_idx++] = v >> 8 * 3;
	}
}

void ByteStream::Write(uint64_t v)
{
	assert(_idx + 8 <= _capacity);

	if (_idx + 8 <= _capacity)
	{
		_buffer[_idx++] = byte(v >> 8 * 0);
		_buffer[_idx++] = byte(v >> 8 * 1);
		_buffer[_idx++] = byte(v >> 8 * 2);
		_buffer[_idx++] = byte(v >> 8 * 3);
		_buffer[_idx++] = byte(v >> 8 * 4);
		_buffer[_idx++] = byte(v >> 8 * 5);
		_buffer[_idx++] = byte(v >> 8 * 6);
		_buffer[_idx++] = byte(v >> 8 * 7);
	}
}

void ByteStream::Write(const byte* data, uint32_t length)
{
	assert(_idx + length <= _capacity);

	if (_idx + length <= _capacity)
	{
		memcpy(_buffer + _idx, data, length);
		_idx += length;
	}
}

void ByteStream::PadWithZero()
{
	if (_capacity - _idx > 0)
	{
		memset(_buffer + _idx, 0, _capacity - _idx);
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