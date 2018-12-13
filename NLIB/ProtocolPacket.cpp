#include "ProtocolPacket.h"

#include <iostream>

#include "Utility.h"

#define READ(variable, type) if(!stream.Read<type>(variable)) { return false; }
#define READ_BYTE(variable, length) if(!stream.Read<const byte*>(variable, length)) { return false; }
#define VALIDATE(expr) if(!(expr)) { return false; }

// TODO FAST FAIL : 현재 상태에 유효하지 않은 패킷 ID는 바로 리턴할 수 있게

ProtocolPacket* ProtocolPacket::Deserialize(ByteStream & stream)
{
	E_PACKET_ID packet_id;
	if (!stream.Read<E_PACKET_ID>(packet_id)) return nullptr;

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

	if (!packet->Read(stream))
		return nullptr;

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

	// TODO Class ConnectToken
	stream.Write(_connect_token_encrypted, NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);
}

bool ProtocolPacketConnectionRequest::Read(ByteStream& stream)
{
	VALIDATE(stream.Remain() == sizeof(_protocol_id) + sizeof(_connect_token_expire) + sizeof(_connect_token_sequence) + NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);

	READ(_protocol_id, uint32_t);
	VALIDATE(_protocol_id == NLIB_PROTOCOL_ID);

	READ(_connect_token_expire, uint64_t);
	VALIDATE(_connect_token_expire < Utility::GetTime());

	READ(_connect_token_sequence, uint64_t);
	READ_BYTE(_connect_token_encrypted, NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);

	return true;
}

void ProtocolPacketConnectionRequest::Print()
{
	std::cout << "Packet ID : Connection Request" << std::endl;
}

void ProtocolPacketConnectionDenied::Write(ByteStream& stream)
{
	stream.Write(GetID());

}

bool ProtocolPacketConnectionDenied::Read(ByteStream& stream)
{
	return true;
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

bool ProtocolPacketConnectionChallenge::Read(ByteStream& stream)
{
	READ(_challenge_token_sequence, uint64_t);
	READ_BYTE(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);

	return true;
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

bool ProtocolPacketConnectionResponse::Read(ByteStream& stream)
{
	READ(_challenge_token_sequence, uint64_t);
	READ_BYTE(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);

	return true;
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

bool ProtocolPacketConnectionKeepAlive::Read(ByteStream& stream)
{
	READ(_client_index, uint32_t);
	READ(_max_clients, uint32_t);

	return true;
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

bool ProtocolPacketConnectionPayload::Read(ByteStream& stream)
{
	_payload_length = stream.Remain();

	READ_BYTE(_payload, _payload_length);

	return true;
}

void ProtocolPacketConnectionPayload::Print()
{
	std::cout << "Packet ID : Connection Payload" << std::endl;
}

void ProtocolPacketConnectionDisconnect::Write(ByteStream& stream)
{
	stream.Write(GetID());

}

bool ProtocolPacketConnectionDisconnect::Read(ByteStream& stream)
{
	return true;
}

void ProtocolPacketConnectionDisconnect::Print()
{
	std::cout << "Packet ID : Connection Disconnect" << std::endl;
}


#undef VALIDATE
#undef READ_BYTE
#undef READ