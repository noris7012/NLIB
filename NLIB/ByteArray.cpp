#include "ByteArray.h"
#include <cassert>
#include <algorithm>

ByteArray::ByteArray(const byte* bytes, uint32_t length)
{
	_bytes = const_cast<byte*>(bytes);
	_capacity = _length = length;
	_read_only = true;
}

ByteArray::ByteArray(uint32_t length)
{
	_bytes = new byte[length];
	_capacity = length;
	_length = 0;
	_read_only = false;
}

ByteArray::~ByteArray()
{
	if (!_read_only)
		delete[] _bytes;
}

void ByteArray::Set(uint32_t index, E_PACKET_ID packet_id)
{
	assert(index >= 0 && index + 1 <= _capacity);
	if (index >= 0 && index + 1 <= _capacity)
	{
		_bytes[index] = static_cast<byte>(packet_id);
		_length = std::max(_length, index + 1);
	}
}

void ByteArray::Set(uint32_t index, uint32_t value)
{
	assert(index >= 0 && index + 4 <= _capacity);
	if (index >= 0 && index + 4 <= _capacity)
	{
		_bytes[index + 0] = value >> 8 * 0;
		_bytes[index + 1] = value >> 8 * 1;
		_bytes[index + 2] = value >> 8 * 2;
		_bytes[index + 3] = value >> 8 * 3;
		_length = std::max(_length, index + 4);
	}
}

void ByteArray::Set(uint32_t index, const ByteArrayPtr& data)
{
	Set(index, data->Bytes(), data->Length());
}

void ByteArray::Set(uint32_t index, byte* bytes, uint32_t length)
{
	if (length == 0)
		return;

	assert(index >= 0 && index + length <= _capacity);
	if (index >= 0 && index + length <= _capacity)
	{
		memcpy(_bytes + index, bytes, length);
		_length = std::max(_length, index + length);
	}
}
