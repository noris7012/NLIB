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
	uint32_t GetSequenceNumber() { return _sequence_number; }
	void Acked() { _acked = true; }
	bool IsAcked() { return _acked; }
	uint32_t GetAckSequenceNumber() { return _ack_sequence_number; }
	uint32_t GetAckBitfield() { return _ack_bitfield; }
	uint64_t GetSendTime() { return _send_time; }

public:
	void Set(uint32_t sequence_number, uint32_t ack_sequence_number, uint32_t ack_bitfield);
	void SetData(const byte* data, uint32_t data_length);
	UNLIBData GetData();

private:
	uint32_t _sequence_number = 1;
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;
	uint64_t _send_time = 0;
	const byte* _data = nullptr;
	uint32_t _data_length = 0;
	bool _acked = false;
};

#endif