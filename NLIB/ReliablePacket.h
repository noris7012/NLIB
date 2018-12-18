#ifndef NLIB_RELIABLE_PACKET_H
#define NLIB_RELIABLE_PACKET_H

#include <stdint.h>

#include "NetworkDefine.h"
#include "ByteStream.h"

class ByteStream;

class ReliablePacket
{
public:
	void Write(ByteStream& stream);
	E_READ_RESULT Read(ByteStream& stream);
	void Print();

public:
	void Set(uint32_t sequence_number, uint32_t ack_sequence_number, uint32_t ack_bitfield);
	void SetData(const byte* data, uint32_t data_length);

private:
	uint32_t _sequence_number;
	uint32_t _ack_sequence_number;
	uint32_t _ack_bitfield;
	const byte* _data;
	uint32_t _data_length;
};

#endif