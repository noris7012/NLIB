#include "ReliablePacket.h"

void ReliablePacket::Write(ByteStream& stream)
{
	stream.Write(_sequence_number);
	stream.Write(_ack_sequence_number);
	stream.Write(_ack_bitfield);
	stream.Write(_data, _data_length);
}

E_READ_RESULT ReliablePacket::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_sequence_number, uint32_t);
	NLIB_STREAM_READ(_ack_sequence_number, uint32_t);
	NLIB_STREAM_READ(_ack_bitfield, uint32_t);

	_data_length = stream.Remain();
	NLIB_STREAM_READ_BYTE(_data, _data_length);

	return E_READ_RESULT::SUCCESS;
}

void ReliablePacket::Print()
{

}

void ReliablePacket::Set(uint32_t sequence_number, uint32_t ack_sequence_number, uint32_t ack_bitfield)
{
	_sequence_number = sequence_number;
	_ack_sequence_number = ack_sequence_number;
	_ack_bitfield = ack_bitfield;
}

void ReliablePacket::SetData(const byte* data, uint32_t data_length)
{
	_data = data;
	_data_length = data_length;
}