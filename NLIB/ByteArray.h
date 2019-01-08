#ifndef NLIB_BYTE_ARRAY_H
#define NLIB_BYTE_ARRAY_H

#include "NetworkDefine.h"

class ByteArray
{
public:
	ByteArray(uint32_t length);
	ByteArray(const byte* bytes, uint32_t length);

	~ByteArray();

public:
	uint32_t Capacity() { return _capacity; }
	uint32_t Length() { return _length; }
	byte* Bytes() { return _bytes; }

public:

public:
	void Set(uint32_t index, E_PACKET_ID packet_id);
	void Set(uint32_t index, uint32_t value);
	void Set(uint32_t index, const ByteArrayPtr& data);
	void Set(uint32_t index, byte* bytes, uint32_t length);

private:
	byte* _bytes;
	uint32_t _capacity;
	uint32_t _length;

	bool _read_only;
};

#endif