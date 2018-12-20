#ifndef NLIB_RELIABLE_PACKET_H
#define NLIB_RELIABLE_PACKET_H

#include <cstdint>

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
	static uint32_t GetHeaderLength() { return sizeof(_sequence_number) + sizeof(_ack_sequence_number) + sizeof(_ack_bitfield); }
	NLIBData GetHeader();

public:
	void Set(uint32_t sequence_number, uint32_t ack_sequence_number, uint32_t ack_bitfield);
	void SetData(const byte* data, uint32_t data_length);
	UNLIBData GetData();

private:
	uint32_t _sequence_number = 1;
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;
	const byte* _data = nullptr;
	uint32_t _data_length = 0;
};

#endif