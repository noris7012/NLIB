#ifndef NLIB_BYTE_STREAM_H
#define NLIB_BYTE_STREAM_H

#include <stdint.h>

#include "NetworkDefine.h"
#include "NetworkStruct.h"

class ByteStream
{
public:
	ByteStream();
	ByteStream(uint32_t capacity);
	ByteStream(const byte* data, uint32_t length);
	ByteStream(Buffer* buffer);
	~ByteStream();

public:
	void Write(E_PACKET_ID packet_id);
	void Write(uint32_t v);
	void Write(uint64_t v);
	void Write(const byte* data, uint32_t length);

	void PadWithZero();

	const byte* Data() const;
	uint32_t Length();
	uint32_t Remain();

private:
	byte* _buffer;
	unsigned int _capacity;
	unsigned int _idx;

public:
	template<class T> T Read()
	{
		assert(false);
		return nullptr;
	}

	template<> E_PACKET_ID Read<E_PACKET_ID>()
	{
		return (E_PACKET_ID)_buffer[_idx++];
	}

	template<> uint32_t Read<uint32_t>()
	{
		uint32_t ret = 0;

		ret += _buffer[_idx++] << 8 * 0;
		ret += _buffer[_idx++] << 8 * 1;
		ret += _buffer[_idx++] << 8 * 2;
		ret += _buffer[_idx++] << 8 * 3;

		return ret;
	}

	template<> uint64_t Read<uint64_t>()
	{
		uint64_t ret = 0;

		ret += ((uint64_t)_buffer[_idx++]) << 8 * 0;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 1;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 2;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 3;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 4;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 5;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 6;
		ret += ((uint64_t)_buffer[_idx++]) << 8 * 7;

		return ret;
	}

	template<class T> T Read(uint32_t)
	{
		assert(false);
		return nullptr;
	}

	template<> const byte* Read<const byte*>(uint32_t length)
	{
		byte* ret = new byte[length];

		memcpy(ret, _buffer + _idx, length);
		_idx += length;

		return ret;
	}
};

#endif
