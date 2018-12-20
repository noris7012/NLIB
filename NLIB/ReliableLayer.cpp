#include "ReliableLayer.h"
#include "ReliablePacket.h"

void ReliableLayer::Read(UNLIBData data)
{
	ByteStream stream(const_cast<byte*>(data->bytes), data->length);
	ReliablePacket packet;
	auto rc = packet.Read(stream);

	if (rc != E_READ_RESULT::SUCCESS)
		return;

	ReadNext(packet.GetData());	
}

void ReliableLayer::Write(UNLIBData data)
{
	ReliablePacket packet;
	packet.Set(_sequence_number++, _ack_sequence_number, _ack_bitfield);

	auto header = packet.GetHeader();
	
	auto new_data = NLIBData::Instance();
	new_data->bytes = header.bytes;
	new_data->length = header.length;
	new_data->next = std::move(data);

	WriteNext(std::move(new_data));
}
