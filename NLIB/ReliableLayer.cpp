#include "ReliableLayer.h"
#include "ReliablePacket.h"
#include "Utility.h"
#include "Logger.h"

ReliableLayer::ReliableLayer(GameEndpoint* endpoint)
	: _endpoint(endpoint)
	, _sequence_number(1)
{
}

void ReliableLayer::Read(PNLIBData data)
{
	ByteStream stream(const_cast<byte*>(data->bytes), data->length);
	ReliablePacket* packet = ReliablePacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	if (packet->GetID() == E_PACKET_ID::RELIABLE_PAYLOAD)
	{
		auto payload = static_cast<ReliablePacketPayload*>(packet);

		// Ignoring Duplicated Packet
		auto old_packet = GetRecvBuffer(payload->GetSequenceNumber());
		if (old_packet != nullptr && old_packet->GetSequenceNumber() == payload->GetSequenceNumber())
			return;

		SetRecvBuffer(payload->GetSequenceNumber(), payload);
		_ack_sequence_number = payload->GetSequenceNumber();

		ReadNext(payload->GetData());
	}
	else if (packet->GetID() == E_PACKET_ID::RELIABLE_ACK)
	{
		auto ack = static_cast<ReliablePacketAck*>(packet);

		auto ack_sequence_number = ack->GetAckSequenceNumber();
		auto ack_bitfield = ack->GetAckBitfield();

		for (int i = 0; i < 32; ++i)
		{
			if (ack_bitfield & 1)
			{
				auto sequence_number = ack_sequence_number - i;
				auto send_packet = GetSendBuffer(sequence_number);

				if (send_packet != nullptr && !send_packet->IsAcked())
				{
					float rtt = (Utility::GetTime() - send_packet->GetSendTime()) / 1000.0f;
					assert(rtt >= 0.0);

					_endpoint->UpdateRTT(rtt);

					send_packet->Acked();
				}
			}

			ack_bitfield >>= 1;
		}
	}
	else
	{
		assert(false);
	}
}

void ReliableLayer::Write(PNLIBData data)
{
	auto packet = new ReliablePacketPayload();
	packet->Set(_sequence_number++);
	auto header = packet->GetHeader();

	packet->SetSendData(data);
	SetSendBuffer(packet->GetSequenceNumber(), packet);
	
	auto new_data = NLIBData::Instance();
	new_data->bytes = header.bytes;
	new_data->length = header.length;
	new_data->next = data;

	WriteNext(new_data);
}

void ReliableLayer::Fail(PNLIBData data)
{
}

void ReliableLayer::Update(uint64_t time)
{
	if (_last_ack_sequence_number != _ack_sequence_number)
	{
		_last_ack_sequence_number = _ack_sequence_number;

		GenerateAck();

		ReliablePacketAck packet;
		packet.Set(_ack_sequence_number, _ack_bitfield);
		auto header = packet.GetHeader();

		auto new_data = NLIBData::Instance();
		new_data->bytes = header.bytes;
		new_data->length = header.length;

		WriteNext(new_data);
	}

	for (uint32_t i = 0; i < NLIB_RELIABLE_BUFFER_SIZE; ++i)
	{
		auto buffer = _send_buffer[i];
		if (buffer == nullptr || buffer->IsAcked())
			continue;

		// TODO acked 패킷은 메모리 Release

		auto rtt = _endpoint->GetRTT();
		if (rtt > 0 && time >= buffer->GetSendTime() + uint64_t(2 * rtt * 1000))
		{
			std::stringstream ss;
			ss << "[Null] " << time << " : " << buffer->GetSendTime() + uint64_t(2 * rtt * 1000) << " (rtt=" << rtt << ") (index=" << buffer->GetSequenceNumber() << ")";
			Logger::GetInstance()->Log(ss.str());

			buffer->Set(_sequence_number++);
			auto header = buffer->GetHeader();

			_send_buffer[i] = nullptr;
			SetSendBuffer(buffer->GetSequenceNumber(), buffer);

			auto new_data = NLIBData::Instance();
			new_data->bytes = header.bytes;
			new_data->length = header.length;
			new_data->next = buffer->GetSendData();

			WriteNext(new_data);
		}
	}
}

ReliablePacketPayload* ReliableLayer::GetSendBuffer(uint32_t sequence_number)
{
	auto packet = _send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))];
	if (packet != nullptr && packet->GetSequenceNumber() != sequence_number)
		return nullptr;

	return packet;
}

void ReliableLayer::SetSendBuffer(uint32_t sequence_number, ReliablePacketPayload* packet)
{
	auto old_packet = _send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))];
	if (old_packet != nullptr && !old_packet->IsAcked())
	{
		FailNext(old_packet->GetData());
	}

	_send_buffer[sequence_number % (sizeof(_send_buffer) / sizeof(*_send_buffer))] = packet;
}

ReliablePacketPayload* ReliableLayer::GetRecvBuffer(uint32_t sequence_number)
{
	auto packet = _recv_buffer[sequence_number % (sizeof(_recv_buffer) / sizeof(*_recv_buffer))];
	if (packet != nullptr && packet->GetSequenceNumber() != sequence_number)
		return nullptr;

	return packet;
}

void ReliableLayer::SetRecvBuffer(uint32_t sequence_number, ReliablePacketPayload* packet)
{
	auto old_packet = _recv_buffer[sequence_number % (sizeof(_recv_buffer) / sizeof(*_recv_buffer))];
	if (old_packet != nullptr)
	{
		// TODO
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
