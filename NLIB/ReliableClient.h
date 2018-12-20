#ifndef NLIB_RELIABLE_CLIENT_H
#define NLIB_RELIABLE_CLIENT_H

#include <cstdint>

#include "NetworkClient.h"
#include "NetworkDefine.h"

#include "ReliablePacket.h"

class ReliableClient
{
public:
	bool Send(const byte* data, uint32_t length);
	void Update(uint64_t time);

private:
	void Send(ReliablePacket& packet);

private:
	uint32_t _sequence_number = 0;
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;

	ReliablePacket* _send_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
	ReliablePacket* _recv_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
};

#endif