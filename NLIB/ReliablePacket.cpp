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

NLIBData ReliablePacketPayload::GetHeader()
{
	ByteStream stream(1 + sizeof(_sequence_number));

	stream.Write(GetID());
	stream.Write(_sequence_number);

	return NLIBData{ stream.Data(), stream.Length(), nullptr };
}

E_READ_RESULT ReliablePacketPayload::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_sequence_number, uint32_t);

	_data_length = stream.Remain();
	NLIB_STREAM_READ_BYTE(_data, _data_length);

	return E_READ_RESULT::SUCCESS;
}

void ReliablePacketPayload::Set(uint32_t sequence_number)
{
	_sequence_number = sequence_number;

	_send_time = Utility::GetTime();
}

PNLIBData ReliablePacketPayload::GetData()
{
	auto data = NLIBData::Instance();
	data->bytes = _data;
	data->length = _data_length;

	return data;
}

void ReliablePacketPayload::SetSendData(PNLIBData send_data)
{
	_send_data = send_data;
}

NLIBData ReliablePacketAck::GetHeader()
{
	ByteStream stream(1 + sizeof(_ack_sequence_number) + sizeof(_ack_bitfield));

	stream.Write(GetID());
	stream.Write(_ack_sequence_number);
	stream.Write(_ack_bitfield);

	return NLIBData{ stream.Data(), stream.Length(), nullptr };
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
