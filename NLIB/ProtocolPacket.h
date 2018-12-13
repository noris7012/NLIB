#ifndef NLIB_PROTOCOL_PACKET_H
#define NLIB_PROTOCOL_PACKET_H

#include "ByteStream.h"
#include "NetworkDefine.h"

class ProtocolPacket
{
public:
	static ProtocolPacket* Deserialize(ByteStream& stream);

public:
	virtual E_PACKET_ID GetID() = 0;
	//virtual ByteStream Serialize() = 0;
	//virtual void Deserialize(ByteStream& stream) = 0;

	virtual void Write(ByteStream& stream) = 0;
	virtual bool Read(ByteStream& stream) = 0;
	virtual void Print() = 0;
};

#define PUBLIC_METHOD(id) \
public: \
	E_PACKET_ID GetID() { return id; } \
	void Write(ByteStream& stream); \
	bool Read(ByteStream& stream); \
	void Print();

class ProtocolPacketConnectionRequest : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_REQUEST)

public:
	ProtocolPacketConnectionRequest();
	~ProtocolPacketConnectionRequest();

private:
	uint32_t _protocol_id;
	uint64_t _connect_token_expire;
	uint64_t _connect_token_sequence;
	const byte* _connect_token_encrypted;
};

class ProtocolPacketConnectionDenied : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_DENIED)
};

class ProtocolPacketConnectionChallenge : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_CHALLENGE)

private:
	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;
};

class ProtocolPacketConnectionResponse : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_RESPONSE)

private:
	uint64_t _challenge_token_sequence;
	const byte* _challenge_token_encrypted;
};

class ProtocolPacketConnectionKeepAlive : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_KEEP_ALIVE)

private:
	uint32_t _client_index;
	uint32_t _max_clients; // TODO Why?
};

class ProtocolPacketConnectionPayload : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_PAYLOAD)

private:
	const byte* _payload;
	uint32_t _payload_length;
};

class ProtocolPacketConnectionDisconnect : public ProtocolPacket
{
	PUBLIC_METHOD(CONNECTION_DISCONNECT)
};

#undef PUBLIC_METHOD

#endif