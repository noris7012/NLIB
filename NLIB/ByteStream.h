#ifndef NLIB_BYTE_STREAM_H
#define NLIB_BYTE_STREAM_H

#include <stdint.h>

#include "NetworkDefine.h"

class ByteStream
{
public:
	ByteStream();
	ByteStream(unsigned int capacity);
	~ByteStream();

public:
	void Write(E_PACKET_ID packet_id);
	void Write(uint32_t v);

	void PadWithZero();

	const unsigned char* Data() const;
	unsigned int Length();

private:
	unsigned char* _buffer;
	unsigned int _capacity;
	unsigned int _idx;
};

#endif
