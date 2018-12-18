#ifndef NLIB_PROTOCOL_PACKET_H
#define NLIB_PROTOCOL_PACKET_H

#include "ByteStream.h"
#include "NetworkDefine.h"
#include "ConnectToken.h"

class ProtocolPacket
{
public:
	static ProtocolPacket* Deserialize(ByteStream& stream);

public:
	virtual E_PACKET_ID GetID() = 0;
	//virtual ByteStream Serialize() = 0;
	//virtual void Deserialize(ByteStream& stream) = 0;

	virtual void Write(ByteStream& stream) = 0;
	virtual E_READ_RESULT Read(ByteStream& stream) = 0;
	virtual void Print() = 0;
};

#define PUBLIC_METHOD(id) \
public: \
	E_PACKET_ID GetID() { return id; } \
	void Write(ByteStream& stream); \
	E_READ_RESULT Read(ByteStream& stream); \
	void Print();

class ProtocolPacketConnectionRequest : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_REQUEST)

public:
	ProtocolPacketConnectionRequest();
	~ProtocolPacketConnectionRequest();

private:
	uint32_t _protocol_id;
	uint64_t _connect_token_expire;
	uint64_t _connect_token_sequence;
	ConnectToken _connect_token;
};

class ProtocolPacketConnectionDenied : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_DENIED);
};

class ProtocolPacketConnectionChallenge : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_CHALLENGE);
	
public:
	~ProtocolPacketConnectionChallenge();

	void Set(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted);

	uint64_t GetChallengeTokenSequence() { return _challenge_token_sequence; }
	const byte* GetChallengeTokenEncrypted() { return _challenge_token_encrypted; }

private:
	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;
};

class ProtocolPacketConnectionResponse : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_RESPONSE)

public:
	void Set(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted);

	uint64_t GetChallengeTokenSequence() { return _challenge_token_sequence; }
	const byte* GetChallengeTokenEncrypted() { return _challenge_token_encrypted; }

private:
	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;
};

class ProtocolPacketConnectionKeepAlive : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_KEEP_ALIVE);

	void Set(uint32_t client_index) 
	{
		_client_index = client_index;
	}

	uint32_t GetClientID() { return _client_index; }

private:
	uint32_t _client_index;
	uint32_t _max_clients; // TODO Why?
};

class ProtocolPacketConnectionPayload : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_PAYLOAD);

	void Set(uint32_t client_index, const byte* payload, uint32_t payload_length)
	{
		_client_index = client_index;
		_payload = payload;
		_payload_length = payload_length;
	}

	uint32_t GetClientID() { return _client_index; }

private:
	uint32_t _client_index;
	const byte* _payload;
	uint32_t _payload_length;
};

class ProtocolPacketConnectionDisconnect : public ProtocolPacket
{
	PUBLIC_METHOD(E_PACKET_ID::CONNECTION_DISCONNECT);

	void Set(uint32_t client_index)
	{
		_client_index = client_index;
	}

	uint32_t GetClientID() { return _client_index; }

private:
	uint32_t _client_index;
};

#undef PUBLIC_METHOD

#endif