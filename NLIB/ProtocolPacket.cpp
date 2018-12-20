#include "ProtocolPacket.h"

#include <iostream>

#include "Utility.h"

// TODO FAST FAIL : 현재 상태에 유효하지 않은 패킷 ID는 바로 리턴할 수 있게

ProtocolPacket* ProtocolPacket::Deserialize(ByteStream & stream)
{
	E_PACKET_ID packet_id;
	if (!stream.Read<E_PACKET_ID>(packet_id)) return nullptr;

	ProtocolPacket* packet = nullptr;
	switch (packet_id)
	{
	case E_PACKET_ID::CONNECTION_REQUEST:
		packet = new ProtocolPacketConnectionRequest();
		break;
	case E_PACKET_ID::CONNECTION_DENIED:
		packet = new ProtocolPacketConnectionDenied();
		break;
	case E_PACKET_ID::CONNECTION_CHALLENGE:
		packet = new ProtocolPacketConnectionChallenge();
		break;
	case E_PACKET_ID::CONNECTION_RESPONSE:
		packet = new ProtocolPacketConnectionResponse();
		break;
	case E_PACKET_ID::CONNECTION_KEEP_ALIVE:
		packet = new ProtocolPacketConnectionKeepAlive();
		break;
	case E_PACKET_ID::CONNECTION_PAYLOAD:
		packet = new ProtocolPacketConnectionPayload();
		break;
	case E_PACKET_ID::CONNECTION_DISCONNECT:
		packet = new ProtocolPacketConnectionDisconnect();
		break;
	default:
		return nullptr;
	}

	auto rc = packet->Read(stream);
	if (rc != E_READ_RESULT::SUCCESS)
		return nullptr;

	return packet;
}

// TODO Read 시에 멤버 변수를 초기화할 필요가 없음.
ProtocolPacketConnectionRequest::ProtocolPacketConnectionRequest()
	: _protocol_id(NLIB_PROTOCOL_ID)
{
	_connect_token_expire = Utility::GetTime() + 60 * 60 * 1000;
	_connect_token_sequence = 0x12345678;
	/*byte* temp = new byte[NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH];

	for (int i = 0; i < NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH; ++i)
	{
		temp[i] = (byte)i;
	}

	_connect_token_encrypted = temp;*/
}

ProtocolPacketConnectionRequest::~ProtocolPacketConnectionRequest()
{
	/*delete[] _connect_token_encrypted;*/
}

void ProtocolPacketConnectionRequest::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_protocol_id);
	stream.Write(_connect_token_expire);
	stream.Write(_connect_token_sequence);

	// TODO Class ConnectToken
	_connect_token.Write(stream);
	/*stream.Write(_connect_token_encrypted, NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH);*/
}

E_READ_RESULT ProtocolPacketConnectionRequest::Read(ByteStream& stream)
{
	NLIB_VALIDATE(stream.Remain() == sizeof(_protocol_id) + sizeof(_connect_token_expire) + sizeof(_connect_token_sequence) + _connect_token.Length(), E_READ_RESULT::FAIL);

	NLIB_STREAM_READ(_protocol_id, uint32_t);
	NLIB_VALIDATE(_protocol_id == NLIB_PROTOCOL_ID, E_READ_RESULT::FAIL);

	NLIB_STREAM_READ(_connect_token_expire, uint64_t);
	NLIB_VALIDATE(_connect_token_expire >= Utility::GetTime(), E_READ_RESULT::FAIL);

	NLIB_STREAM_READ(_connect_token_sequence, uint64_t);
	NLIB_VALIDATE_FUNC(_connect_token.Read(stream));

	return E_READ_RESULT::SUCCESS;
}

void ProtocolPacketConnectionRequest::Print()
{
	std::cout << "Packet ID : Connection Request" << std::endl;
}

void ProtocolPacketConnectionDenied::Write(ByteStream& stream)
{
	stream.Write(GetID());

}

E_READ_RESULT ProtocolPacketConnectionDenied::Read(ByteStream& stream)
{
	return E_READ_RESULT::SUCCESS;
}

void ProtocolPacketConnectionDenied::Print()
{
	std::cout << "Packet ID : Connection Denied" << std::endl;
}

ProtocolPacketConnectionChallenge::~ProtocolPacketConnectionChallenge()
{
}

void ProtocolPacketConnectionChallenge::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_challenge_token_sequence);
	stream.Write(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);

	//std::cout << "[ Write Challenge Token ] " << std::endl;
	//std::cout << Utility::ByteToString(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH) << std::endl;
}

void ProtocolPacketConnectionChallenge::Set(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted)
{
	_challenge_token_sequence = challenge_token_sequence;
	_challenge_token_encrypted = challenge_token_encrypted;
}

E_READ_RESULT ProtocolPacketConnectionChallenge::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_challenge_token_sequence, uint64_t);
	NLIB_STREAM_READ_BYTE(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);

	//std::cout << "[ Read Challenge Token ] " << std::endl;
	//std::cout << Utility::ByteToString(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH) << std::endl;

	return E_READ_RESULT::SUCCESS;
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

void ProtocolPacketConnectionResponse::Set(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted)
{
	_challenge_token_sequence = challenge_token_sequence;
	_challenge_token_encrypted = challenge_token_encrypted;
}

E_READ_RESULT ProtocolPacketConnectionResponse::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_challenge_token_sequence, uint64_t);
	NLIB_STREAM_READ_BYTE(_challenge_token_encrypted, NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH);

	return E_READ_RESULT::SUCCESS;
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

E_READ_RESULT ProtocolPacketConnectionKeepAlive::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_client_index, uint32_t);
	NLIB_STREAM_READ(_max_clients, uint32_t);

	return E_READ_RESULT::SUCCESS;
}

void ProtocolPacketConnectionKeepAlive::Print()
{
	std::cout << "Packet ID : Connection Keyy Alive" << std::endl;
}

void ProtocolPacketConnectionPayload::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_client_index);
	stream.Write(_payload, _payload_length);
}

E_READ_RESULT ProtocolPacketConnectionPayload::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_client_index, uint32_t);

	_payload_length = stream.Remain();
	NLIB_STREAM_READ_BYTE(_payload, _payload_length);

	return E_READ_RESULT::SUCCESS;
}

void ProtocolPacketConnectionPayload::Print()
{
	std::cout << "Packet ID : Connection Payload" << std::endl;
}

UNLIBData ProtocolPacketConnectionPayload::GetPayload()
{
	auto data = NLIBData::Instance();
	data->bytes = _payload;
	data->length = _payload_length;

	return data;
}

void ProtocolPacketConnectionDisconnect::Write(ByteStream& stream)
{
	stream.Write(GetID());

	stream.Write(_client_index);
}

E_READ_RESULT ProtocolPacketConnectionDisconnect::Read(ByteStream& stream)
{
	NLIB_STREAM_READ(_client_index, uint32_t);

	return E_READ_RESULT::SUCCESS;
}

void ProtocolPacketConnectionDisconnect::Print()
{
	std::cout << "Packet ID : Connection Disconnect" << std::endl;
}