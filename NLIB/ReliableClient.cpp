#include "ReliableClient.h"

bool ReliableClient::Send(const byte* data, uint32_t length)
{
	// TODO length check

	if (!IsConnected())
	{
		// delete[] data
		return false;
	}

	ReliablePacket packet;
	packet.Set(_sequence_number++, _ack_sequence_number, _ack_bitfield);
	packet.SetData(data, length);

	auto buffer = _buffer_pool.Acquire();
	ByteStream stream(buffer);
	packet.Write(stream);
	NetworkClient::SendPacket(stream.Data(), stream.Length());
	_buffer_pool.Release(buffer);

	return true;
}

//bool ReliableClient::Read(const byte* data, uint32_t length)
//{
//	ByteStream stream((byte*)data, length);
//	ReliablePacket packet;
//	packet.Read(stream);
//
//
//}

void ReliableClient::Update(uint64_t time)
{
	NetworkClient::Update(time);

	// Ack
}