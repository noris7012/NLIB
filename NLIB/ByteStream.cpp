#include "ByteStream.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

ByteStream::ByteStream()
	: ByteStream(1024)
{
}

ByteStream::ByteStream(unsigned int capacity)
{
	_buffer = new unsigned char[capacity];
	_capacity = capacity;
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

void ByteStream::PadWithZero()
{
	if (_capacity - _idx > 0)
	{
		memset(_buffer + _idx, 0, _capacity - _idx);
		_idx = _capacity;
	}
}

const unsigned char* ByteStream::Data() const
{
	return _buffer;
}

unsigned int ByteStream::Length()
{
	return _idx;
}