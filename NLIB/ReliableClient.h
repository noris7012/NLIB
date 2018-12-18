#ifndef NLIB_RELIABLE_CLIENT_H
#define NLIB_RELIABLE_CLIENT_H

#include <stdint.h>

#include "NetworkClient.h"
#include "NetworkDefine.h"

#include "ReliablePacket.h"

class ReliableClient : public NetworkClient
{
public:
	bool Send(const byte* data, uint32_t length);
	void Update(uint64_t time) override;

private:
	void Send(ReliablePacket& packet);

private:
	uint32_t _sequence_number;
	uint32_t _ack_sequence_number;
	uint32_t _ack_bitfield;

	ReliablePacket* _send_buffer[NLIB_RELIABLE_BUFFER_SIZE];
	ReliablePacket* _recv_buffer[NLIB_RELIABLE_BUFFER_SIZE];
};

#endif
