#include "ReliablePacket.h"
#include "Utility.h"

ReliablePacket* ReliablePacket::Deserialize(ByteStream& stream)
{
	E_PACKET_ID packet_id;
	if (!stream.Read<E_PACKET_ID>(packet_id)) return nullptr;

	ReliablePacket* packet = nullptr;
	switch (packet_id)
	{
	case E_PACKET_ID::RELIABLE_PAYLOAD:
		packet = new ReliablePacketPayload();
		break;
	case E_PACKET_ID::RELIABLE_ACK:
		packet = new ReliablePacketAck();
		break;
	default:
		return nullptr;
	}

	auto rc = packet->Read(stream);
	if (rc != E_READ_RESULT::SUCCESS)
		return nullptr;

	return packet;
}

void ReliablePacketPayload::WriteHeader(ByteArrayPtr data)
{
	data->Set(NLIB_OFFSET_RELIABLE, GetID());
	data->Set(NLIB_OFFSET_RELIABLE+ 1, _sequence_number);
}

E_READ_RESULT ReliablePacketPayload::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_sequence_number, uint32_t);

	_data = stream.GetData();

	return E_READ_RESULT::SUCCESS;
}

void ReliablePacketPayload::Set(uint32_t sequence_number)
{
	_sequence_number = sequence_number;

	_send_time = Utility::GetTime();
}

void ReliablePacketAck::WriteHeader(ByteArrayPtr data)
{
	data->Set(NLIB_OFFSET_RELIABLE + 0, GetID());
	data->Set(NLIB_OFFSET_RELIABLE + 1, _ack_sequence_number);
	data->Set(NLIB_OFFSET_RELIABLE + 5, _ack_bitfield);
}

E_READ_RESULT ReliablePacketAck::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_ack_sequence_number, uint32_t);
	NLIB_STREAM_READ(_ack_bitfield, uint32_t);

	return E_READ_RESULT::SUCCESS;
}

void ReliablePacketAck::Set(uint32_t ack_sequence_number, uint32_t ack_bitfield)
{
	_ack_sequence_number = ack_sequence_number;
	_ack_bitfield = ack_bitfield;
}
