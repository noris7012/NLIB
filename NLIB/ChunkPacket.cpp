#include "ChunkPacket.h"

ChunkPacket* ChunkPacket::Deserialize(ByteStream& stream)
{
	E_PACKET_ID packet_id;
	if (!stream.Read<E_PACKET_ID>(packet_id)) return nullptr;

	ChunkPacket* packet = nullptr;
	switch (packet_id)
	{
	case E_PACKET_ID::CHUNK_NONE:
		packet = new ChunkPacketNone();
		break;
	case E_PACKET_ID::CHUNK_SOME:
		packet = new ChunkPacketSome();
		break;
	default:
		return nullptr;
	}

	auto rc = packet->Read(stream);
	if (rc != E_READ_RESULT::SUCCESS)
		return nullptr;

	return packet;
}

ChunkPacket::ChunkPacket()
{
}

NLIBData ChunkPacketNone::GetHeader()
{
	ByteStream stream(1);

	stream.Write(GetID());

	return NLIBData{ stream.Data(), stream.Length(), nullptr };
}

E_READ_RESULT ChunkPacketNone::Read(ByteStream& stream)
{
	_data_length = stream.Remain();
	NLIB_STREAM_READ_BYTE(_data, _data_length);

	return E_READ_RESULT::SUCCESS;
}

PNLIBData ChunkPacketNone::GetData()
{
	auto data = NLIBData::Instance();
	data->bytes = _data;
	data->length = _data_length;

	return data;
}

NLIBData ChunkPacketSome::GetHeader()
{
	ByteStream stream(1);

	stream.Write(GetID());

	return NLIBData{ stream.Data(), stream.Length(), nullptr };
}

E_READ_RESULT ChunkPacketSome::Read(ByteStream& stream)
{
	_data_length = stream.Remain();
	NLIB_STREAM_READ_BYTE(_data, _data_length);

	return E_READ_RESULT::SUCCESS;
}

PNLIBData ChunkPacketSome::GetSendData()
{
	return _send_data;
}

void ChunkPacketSome::SetSendData(PNLIBData send_data)
{
	_send_data = send_data;
}
