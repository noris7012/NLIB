#include "ReliableLayer.h"
#include "ReliablePacket.h"
#include "Utility.h"

ReliableLayer::ReliableLayer(GameEndpoint* endpoint)
	: _endpoint(endpoint)
{
}

void ReliableLayer::Read(UNLIBData data)
{
	ByteStream stream(const_cast<byte*>(data->bytes), data->length);
	ReliablePacket* packet = new ReliablePacket();
	auto rc = packet->Read(stream);

	if (rc != E_READ_RESULT::SUCCESS)
		return;

	auto ack_sequence_number = packet->GetAckSequenceNumber();
	auto ack_bitfield = packet->GetAckBitfield();

	for (int i = 0; i < 32; ++i)
	{
		if (ack_bitfield & 1)
		{
			auto send_packet = GetSendBuffer(ack_sequence_number - i);

			if (send_packet != nullptr && !send_packet->IsAcked())
			{
				float rtt = ( Utility::GetTime() - send_packet->GetSendTime() ) / 1000.0f;
				assert(rtt >= 0.0);

				_endpoint->UpdateRTT(rtt);

				send_packet->Acked();
			}
		}

		ack_bitfield >>= 1;
	}

	SetRecvBuffer(packet->GetSequenceNumber(), packet);
	_ack_sequence_number = packet->GetSequenceNumber();

	ReadNext(packet->GetData());	
}

void ReliableLayer::Write(UNLIBData data)
{
	GenerateAck();

	ReliablePacket* packet = new ReliablePacket();
	packet->Set(_sequence_number++, _ack_sequence_number, _ack_bitfield);

	auto header = packet->GetHeader();

	SetSendBuffer(packet->GetSequenceNumber(), packet);
	
	auto new_data = NLIBData::Instance();
	new_data->bytes = header.bytes;
	new_data->length = header.length;
	new_data->next = std::move(data);

	WriteNext(std::move(new_data));
}

ReliablePacket* ReliableLayer::GetSendBuffer(uint32_t sequence_number)
{
	auto packet = _send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))];
	if (packet != nullptr && packet->GetSequenceNumber() != sequence_number)
		return nullptr;

	return packet;
}

void ReliableLayer::SetSendBuffer(uint32_t sequence_number, ReliablePacket* packet)
{
	auto old_packet = _send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))];
	if (old_packet != nullptr && !old_packet->IsAcked())
	{
		assert(false);
		// TODO Handle Fail
	}

	_send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))] = packet;
}

ReliablePacket* ReliableLayer::GetRecvBuffer(uint32_t sequence_number)
{
	auto packet = _recv_buffer[sequence_number % (sizeof(_recv_buffer) / sizeof(*_recv_buffer))];
	if (packet != nullptr && packet->GetSequenceNumber() != sequence_number)
		return nullptr;

	return packet;
}

void ReliableLayer::SetRecvBuffer(uint32_t sequence_number, ReliablePacket* packet)
{
	auto old_packet = _recv_buffer[sequence_number % (sizeof(_recv_buffer) / sizeof(*_recv_buffer))];
	if (old_packet != nullptr)
	{
		delete old_packet;
	}

	_recv_buffer[sequence_number % (sizeof(_recv_buffer) / sizeof(*_recv_buffer))] = packet;
}

void ReliableLayer::GenerateAck()
{
	auto mask = 1;
	uint32_t bitfield = 0;

	for (int i = 0; i < 32; ++i)
	{
		if (GetRecvBuffer(_ack_sequence_number - i) != nullptr)
			bitfield |= mask;

		mask <<= 1;
	}

	_ack_bitfield = bitfield;
}
