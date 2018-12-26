#ifndef NLIB_NETWORK_DEFINE_H
#define NLIB_NETWORK_DEFINE_H

#include <memory.h>
#include <climits>
#include <memory>

//#define MAX_NETWORK_ENDPOINTS_PER_THREAD 4;


#define MAX_MTU_SIZE 1024

#define NLIB_MAX_SESSION 2048
#define NLIB_MAX_CONNECTION_SLOT 1024

#define NLIB_PROTOCOL_ID 0x41d93c7f

// TODO Connect Token
#define NLIB_CONNECT_TOKEN new byte[20]
#define NLIB_CONNECT_TOKEN_LENGTH sizeof(NLIB_CONNECT_TOKEN)
#define NLIB_CONNECT_TIMEOUT 5000
//#define NLIB_CONNECT_TOKEN_ENCRYPTED_LENGTH 896
#define NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH 300

//#define NLIB_LOG_ENABLED

#define NLIB_STREAM_READ(variable, type) if(!stream.Read<type>(variable)) { return E_READ_RESULT::FAIL; }
#define NLIB_STREAM_READ_BYTE(variable, length) if(!stream.Read<const byte*>(variable, length)) { return E_READ_RESULT::FAIL; }

#define NLIB_VALIDATE(expr, result) if(!(expr)) { return result; }
#define NLIB_VALIDATE_FUNC(func) \
	{ \
		auto _rc = func; \
		if (_rc != E_READ_RESULT::SUCCESS) return _rc; \
	}

#define NLIB_RELIABLE_BUFFER_SIZE 256

enum class E_CLIENT_STATE_ID
{
	INIT = 0,
	DISCONNECTED,
	SENDING_CONNECTION_REQUEST,
	SENDING_CONNECTION_RESPONSE,
	CONNECTED,

	MAX_CLEINT_STATE_ID,
};

enum class E_SESSION_STATE_ID
{
	INIT = 0,
	DISCONNECTED,
	SENDING_CONNECTION_CHALLENGE,
	CONNECTED,

	MAX_SESSION_STATE_ID,
};

enum class E_PACKET_ID
{
	CONNECTION_REQUEST = 0,
	CONNECTION_DENIED,
	CONNECTION_CHALLENGE,
	CONNECTION_RESPONSE,
	CONNECTION_KEEP_ALIVE,
	CONNECTION_PAYLOAD,
	CONNECTION_DISCONNECT,

	RELIABLE_PAYLOAD,
	RELIABLE_ACK,

	MAX_PACKET_ID,
};

enum class E_TRANSPORT_TYPE
{
	UDP,
	TCP,
};

enum class E_READ_RESULT
{
	FAIL = INT_MIN,
	INVALID_TOKEN,

	SUCCESS = 0,
};

using byte = unsigned char;

class GameClient;
using PGameClient = std::shared_ptr<GameClient>;

class GamePacket;
using PGamePacket = std::shared_ptr<GamePacket>;

class GameSession;
using PGameSession = std::shared_ptr<GameSession>;

class GameServerHandler;
using PGameServerHandler = std::shared_ptr<GameServerHandler>;

class GameClientHandler;
using PGameClientHandler = std::shared_ptr<GameClientHandler>;

#endif