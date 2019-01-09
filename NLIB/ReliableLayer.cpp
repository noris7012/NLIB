#include "ReliableLayer.h"
#include "ReliablePacket.h"
#include "Utility.h"
#include "Logger.h"

ReliableLayer::ReliableLayer(GameEndpoint* endpoint)
	: _endpoint(endpoint)
	, _sequence_number(1)
{
}

void ReliableLayer::Read(const ReadParam& param)
{
	auto data = param.data;

	ByteStream stream(data, NLIB_OFFSET_RELIABLE);
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

		ReadNext(ReadParam{ payload->GetData() });
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

void ReliableLayer::Write(const WriteParam& param)
{
	auto data = param.data;

	auto packet = new ReliablePacketPayload();
	packet->Set(_sequence_number++);
	packet->SetData(data);
	packet->WriteHeader(data);

	SetSendBuffer(packet->GetSequenceNumber(), packet);

	WriteNext(param);
}

void ReliableLayer::Fail(const FailParam& param)
{
}

void ReliableLayer::Update(uint64_t time)
{
	if (_last_ack_sequence_number != _ack_sequence_number)
	{
		_last_ack_sequence_number = _ack_sequence_number;

		GenerateAck();

		auto data = std::make_shared<ByteArray>(MAX_MTU_SIZE);

		ReliablePacketAck packet;
		packet.Set(_ack_sequence_number, _ack_bitfield);
		packet.WriteHeader(data);

		WriteNext(WriteParam{ data });
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

			auto data = buffer->GetData();

			buffer->Set(_sequence_number++);
			buffer->WriteHeader(data);

			_send_buffer[i] = nullptr;
			SetSendBuffer(buffer->GetSequenceNumber(), buffer);

			WriteNext(WriteParam{ data });
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
		FailNext(FailParam{ old_packet->GetData() });
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
