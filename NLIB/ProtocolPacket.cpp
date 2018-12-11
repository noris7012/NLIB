#include "ProtocolPacket.h"

#include <iostream>

#include "Utility.h"

ProtocolPacket* ProtocolPacket::Deserialize(ByteStream & stream)
{
	auto packet_id = stream.Read<E_PACKET_ID>();

	ProtocolPacket* packet = nullptr;
	switch (packet_id)
	{
	case CONNECTION_REQUEST:
		packet = new ProtocolPacketConnectionRequest();
		break;
	case CONNECTION_DENIED:
		packet = new ProtocolPacketConnectionDenied();
		break;
	case CONNECTION_CHALLENGE:
		packet = new ProtocolPacketConnectionChallenge();
		break;
	case CONNECTION_RESPONSE:
		packet = new ProtocolPacketConnectionResponse();
		break;
	case CONNECTION_KEEP_ALIVE:
		packet = new ProtocolPacketConnectionKeepAlive();
		break;
	case CONNECTION_PAYLOAD:
		packet = new ProtocolPacketConnectionPayload();
		break;
	case CONNECTION_DISCONNECT:
		packet = new ProtocolPacketConnectionDisconnect();
		break;
	default:
		return nullptr;
	}

	packet->Read(stream);

	return packet;
}

ProtocolPacketConnectionRequest::ProtocolPacketConnectionRequest()
	: _protocol_id(NLIB_PROTOCOL_ID)
{
	_connect_token_expire = Utility::GetTime() + 60 * 60 * 1000;
	_connect_token_sequence = 0x12345678;
	byte* temp = new byte[NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH];

	for (int i = 0; i < NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH; ++i)
	{
		temp[i] = (byte)i;
	}

	_connect_token_encrypted = temp;
}

ProtocolPacketConnectionRequest::~ProtocolPacketConnectionRequest()
{
	delete[] _connect_token_encrypted;
}

void ProtocolPacketConnectionRequest::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_protocol_id);
	stream.Write(_connect_token_expire);
	stream.Write(_connect_token_sequence);
	stream.Write(_connect_token_encrypted, NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionRequest::Read(ByteStream& stream)
{
	_protocol_id = stream.Read<uint32_t>();
	_connect_token_expire = stream.Read<uint64_t>();
	_connect_token_sequence = stream.Read<uint64_t>();
	_connect_token_encrypted = stream.Read<const byte*>(NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionRequest::Print()
{
	std::cout << "Packet ID : Connection Request" << std::endl;
}

void ProtocolPacketConnectionDenied::Write(ByteStream& stream)
{
	stream.Write(GetID());

}

void ProtocolPacketConnectionDenied::Read(ByteStream& stream)
{

}

void ProtocolPacketConnectionDenied::Print()
{
	std::cout << "Packet ID : Connection Denied" << std::endl;
}

void ProtocolPacketConnectionChallenge::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_challenge_token_sequence);
	stream.Write(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionChallenge::Read(ByteStream& stream)
{
	_challenge_token_sequence = stream.Read<uint64_t>();
	_challenge_token_encrypted = stream.Read<const byte*>(NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionChallenge::Print()
{
	std::cout << "Packet ID : Connection Challenge" << std::endl;
}

void ProtocolPacketConnectionResponse::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_challenge_token_sequence);
	stream.Write(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionResponse::Read(ByteStream& stream)
{
	_challenge_token_sequence = stream.Read<uint64_t>();
	_challenge_token_encrypted = stream.Read<const byte*>(NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);
}

void ProtocolPacketConnectionResponse::Print()
{
	std::cout << "Packet ID : Connection Response" << std::endl;
}

void ProtocolPacketConnectionKeepAlive::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_client_index);
	stream.Write(_max_clients);
}

void ProtocolPacketConnectionKeepAlive::Read(ByteStream& stream)
{
	_client_index = stream.Read<uint32_t>();
	_max_clients = stream.Read<uint32_t>();
}

void ProtocolPacketConnectionKeepAlive::Print()
{
	std::cout << "Packet ID : Connection Keyy Alive" << std::endl;
}

void ProtocolPacketConnectionPayload::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_payload, _payload_length);
}

void ProtocolPacketConnectionPayload::Read(ByteStream& stream)
{
	_payload_length = stream.Remain();
	_payload = stream.Read<const byte*>(_payload_length);
}

void ProtocolPacketConnectionPayload::Print()
{
	std::cout << "Packet ID : Connection Payload" << std::endl;
}

void ProtocolPacketConnectionDisconnect::Write(ByteStream& stream)
{
	stream.Write(GetID());

}

void ProtocolPacketConnectionDisconnect::Read(ByteStream& stream)
{

}

void ProtocolPacketConnectionDisconnect::Print()
{
	std::cout << "Packet ID : Connection Disconnect" << std::endl;
}