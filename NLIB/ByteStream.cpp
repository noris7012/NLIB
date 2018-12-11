#include "ByteStream.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

ByteStream::ByteStream()
	: ByteStream(1024)
{
}

ByteStream::ByteStream(uint32_t capacity)
{
	_buffer = new byte[capacity];
	_capacity = capacity;
	_idx = 0;
}


// TODO 카피 해야 할 때와 안해도 될 때 구분하기
ByteStream::ByteStream(const byte* data, uint32_t length)
{
	_buffer = new byte[length];
	_capacity = length;
	_idx = 0;

	memcpy_s(_buffer, length, data, length);
}

ByteStream::ByteStream(Buffer* buffer)
{
	_buffer = buffer->data;
	_capacity = sizeof(buffer->data);
	_idx = 0;
}

ByteStream::~ByteStream()
{
	delete[] _buffer;
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
		_buffer[_idx++] = v >> 8 * 0;
		_buffer[_idx++] = v >> 8 * 1;
		_buffer[_idx++] = v >> 8 * 2;
		_buffer[_idx++] = v >> 8 * 3;
		_buffer[_idx++] = v >> 8 * 4;
		_buffer[_idx++] = v >> 8 * 5;
		_buffer[_idx++] = v >> 8 * 6;
		_buffer[_idx++] = v >> 8 * 7;
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

const byte* ByteStream::Data() const
{
	return _buffer;
}

uint32_t ByteStream::Length()
{
	return _idx;
}

uint32_t ByteStream::Remain()
{
	return _capacity - _idx;
}