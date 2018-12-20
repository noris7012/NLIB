#ifndef NLIB_RELIABLE_SESSION_H
#define NLIB_RELIABLE_SESSION_H

#include <cstdint>

#include "NetworkLayer.h"
#include "NetworkSession.h"
#include "NetworkStruct.h"
#include "ReliablePacket.h"

class NetworkLayer;

class ReliableSession : public NetworkLayer
{
public:
	void Read(UNLIBData data) override;
	void Write(UNLIBData data) override;

private:
	uint32_t _sequence_number = 0;
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;

	ReliablePacket* _send_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
	ReliablePacket* _recv_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
};

#endif