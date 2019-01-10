#ifndef NLIB_RELIABLE_PACKET_H
#define NLIB_RELIABLE_PACKET_H

#include <cstdint>

#include "NetworkDefine.h"
#include "ByteArray.h"
#include "ByteStream.h"

class ByteStream;

class ReliablePacket
{
public:
	static ReliablePacket* Deserialize(ByteStream& stream);

public:
	virtual E_PACKET_ID GetID() = 0;

	virtual void WriteHeader(ByteArrayPtr data) = 0;
	virtual E_READ_RESULT Read(ByteStream& stream) = 0;
	virtual void Print() { };
};

class ReliablePacketPayload : public ReliablePacket
{
public:
	~ReliablePacketPayload();

public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::RELIABLE_PAYLOAD; }
	void WriteHeader(ByteArrayPtr data) override;
	E_READ_RESULT Read(ByteStream& stream) override;

public:
	void Set(uint32_t sequence_number);

	void SetData(ByteArrayPtr data) { _data = data; }
	ByteArrayPtr GetData() { return _data; }

	uint32_t GetSequenceNumber() { return _sequence_number; }
	void Acked() { _acked = true; }
	bool IsAcked() { return _acked; }
	uint64_t GetSendTime() { return _send_time; }
	void SetSendTime(uint64_t send_time) { _send_time = send_time; }
	void SetRetryLimit(uint32_t retry_limit) { _retry_limit = retry_limit; }

	bool NeedRetry() { return _retry_count < _retry_limit; }
	void IncRetryCount() { ++_retry_count; }

private:
	ByteArrayPtr _data;

	uint32_t _sequence_number = 0;
	uint64_t _send_time = 0;
	bool _acked = false;

	uint32_t _retry_limit = 0;
	uint32_t _retry_count = 0;
};

class ReliablePacketAck : public ReliablePacket
{
public:
	E_PACKET_ID GetID() override { return E_PACKET_ID::RELIABLE_ACK; }
	void WriteHeader(ByteArrayPtr data) override;
	E_READ_RESULT Read(ByteStream& stream) override;

public:
	void Set(uint32_t ack_sequence_number, uint32_t ack_bitfield);
	uint32_t GetAckSequenceNumber() { return _ack_sequence_number; }
	uint32_t GetAckBitfield() { return _ack_bitfield; }

private:
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;
};

#endif