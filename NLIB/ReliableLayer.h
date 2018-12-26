#ifndef NLIB_RELIABLE_SESSION_H
#define NLIB_RELIABLE_SESSION_H

#include <cstdint>

#include "GameEndpoint.h"
#include "NetworkLayer.h"
#include "NetworkSession.h"
#include "NetworkStruct.h"
#include "ReliablePacket.h"

class GameEndpoint;
class NetworkLayer;

class ReliableLayer : public NetworkLayer
{
public:
	ReliableLayer(GameEndpoint* endpoint);

public:
	void Read(UNLIBData data) override;
	void Write(UNLIBData data) override;
	void Update(uint64_t time);

	ReliablePacketPayload* GetSendBuffer(uint32_t sequence_number);
	void SetSendBuffer(uint32_t sequence_number, ReliablePacketPayload* packet);

	ReliablePacketPayload* GetRecvBuffer(uint32_t sequence_number);
	void SetRecvBuffer(uint32_t sequence_number, ReliablePacketPayload* packet);

	void GenerateAck();

private:
	GameEndpoint* _endpoint;
	uint32_t _sequence_number = 0;
	uint32_t _ack_sequence_number = 0;
	uint32_t _ack_bitfield = 0;

	uint32_t _last_ack_sequence_number = 0;

	ReliablePacketPayload* _send_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
	ReliablePacketPayload* _recv_buffer[NLIB_RELIABLE_BUFFER_SIZE] = {};
};

#endif