#include "NetworkClient.h"

#include <iostream>

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>

#include "Utility.h"

NetworkClient::NetworkClient()
	: _state(NULL), _state_map(), _state_transition_table()
{
#define STATE_CREATE(id) ( _state_map[E_CLIENT_STATE_ID::id] = ClientState::create(E_CLIENT_STATE_ID::id, this) )
#define TRANSITION_CREATE(id, ...) ( _state_transition_table[E_CLIENT_STATE_ID::id] = new std::vector<E_CLIENT_STATE_ID>{ ##__VA_ARGS__ } )

	STATE_CREATE(INIT);
	TRANSITION_CREATE(INIT, E_CLIENT_STATE_ID::SENDING_CONNECTION_REQUEST);

	STATE_CREATE(DISCONNECTED);
	TRANSITION_CREATE(DISCONNECTED, E_CLIENT_STATE_ID::INIT);

	STATE_CREATE(SENDING_CONNECTION_REQUEST);
	TRANSITION_CREATE(SENDING_CONNECTION_REQUEST, E_CLIENT_STATE_ID::DISCONNECTED, E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE);

	STATE_CREATE(SENDING_CONNECTION_RESPONSE);
	TRANSITION_CREATE(SENDING_CONNECTION_RESPONSE, E_CLIENT_STATE_ID::DISCONNECTED, E_CLIENT_STATE_ID::CONNECTED);

	STATE_CREATE(CONNECTED);
	TRANSITION_CREATE(CONNECTED, E_CLIENT_STATE_ID::DISCONNECTED);

#undef TRANSITIN_CREATE
#undef STATE_CREATE

	SetState(E_CLIENT_STATE_ID::INIT);
}

NetworkClient::~NetworkClient()
{
	// TODO delete 잘 되는지 확인해봐야함
	if (_challenge_token_encrypted != nullptr)
		delete[] _challenge_token_encrypted;
}

bool NetworkClient::Connect(const char* host, unsigned short port)
{
	// TODO Check State
	NetworkConfig config;
	config.transport_type = E_TRANSPORT_TYPE::UDP;
	config.host = host;
	config.port = port;

	Startup(config);

	return SetState(E_CLIENT_STATE_ID::SENDING_CONNECTION_REQUEST);
}

bool NetworkClient::IsConnected()
{
	assert(_state != nullptr);
	return _state->getID() == E_CLIENT_STATE_ID::CONNECTED;
}

void NetworkClient::Disconnect()
{
	SetState(E_CLIENT_STATE_ID::DISCONNECTED);
}

void NetworkClient::Update(uint64_t time)
{
	if (_state != nullptr)
	{
		_state->Update(time);
	}
}

void NetworkClient::ProcessReceive(NLIBRecv* recv)
{
	assert(_state != nullptr);
	if (_state == nullptr)
		return;

#ifdef NLIB_LOG_ENABLED
	std::cout << "[ Client Receive ] " << std::endl;
	std::cout << Utility::ByteToString(recv->buffer->data, recv->length) << std::endl;
#endif

	ByteStream stream(recv->buffer->data, recv->length);
	ProtocolPacket* packet = ProtocolPacket::Deserialize(stream);

	assert(packet != nullptr);
	if (packet == nullptr)
		return;

	packet->Print();

	_state->RecvPacket(packet);
	
	delete packet;
}

void NetworkClient::SendPacket(const byte* data, uint32_t length)
{
	assert(_state != nullptr);
	if (_state == nullptr)
		return;

	_state->SendPacket(data, length);
}

bool NetworkClient::SetState(E_CLIENT_STATE_ID state_id)
{
	if (_state == nullptr)
	{
		_state = _state_map[state_id];
		_state->OnEnter();
		return true;
	}
	else
	{
		auto vect = _state_transition_table[_state->getID()];
		for (auto it = vect->begin(); it != vect->end(); ++it)
		{
			if (*it == state_id)
			{
				_state->OnExit();
				_state = _state_map[state_id];
				_state->OnEnter();
				return true;
			}
		}
		return false;
	}
}

void NetworkClient::SetChallengeToken(uint64_t challenge_token_sequence, const byte* challenge_token_encrypted)
{
	_challenge_token_sequence = challenge_token_sequence;
	_challenge_token_encrypted = challenge_token_encrypted;
}