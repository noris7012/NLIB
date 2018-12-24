#include "ReliablePacket.h"
#include "Utility.h"

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

NLIBData ReliablePacket::GetHeader()
{
	ByteStream stream(GetHeaderLength());

	stream.Write(_sequence_number);
	stream.Write(_ack_sequence_number);
	stream.Write(_ack_bitfield);

	return NLIBData{ stream.Data(), stream.Length(), nullptr };
}

void ReliablePacket::Set(uint32_t sequence_number, uint32_t ack_sequence_number, uint32_t ack_bitfield)
{
	_sequence_number = sequence_number;
	_ack_sequence_number = ack_sequence_number;
	_ack_bitfield = ack_bitfield;

	_send_time = Utility::GetTime();
}

void ReliablePacket::SetData(const byte* data, uint32_t data_length)
{
	_data = data;
	_data_length = data_length;
}

UNLIBData ReliablePacket::GetData()
{
	auto data = NLIBData::Instance();
	data->bytes = _data;
	data->length = _data_length;

	return data;
}
