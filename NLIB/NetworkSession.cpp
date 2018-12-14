#include "NetworkSession.h"

#include "Utility.h"

NetworkSession::NetworkSession(NetworkServer* server, uint64_t challenge_token_sequence, const byte* challenge_token_encrypted, NLIBAddress address)
	: _server(server)
	, _challenge_token_sequence(challenge_token_sequence)
	, _challenge_token_encrypted(challenge_token_encrypted)
	, _address(address)
{
	_created_time = Utility::GetTime();

#define STATE_CREATE(id) ( _state_map[E_SESSION_STATE_ID::id] = SessionState::Create(E_SESSION_STATE_ID::id, this) )
#define TRANSITION_CREATE(id, ...) ( _state_transition_table[E_SESSION_STATE_ID::id] = new std::vector<E_SESSION_STATE_ID>{ ##__VA_ARGS__ } )

	STATE_CREATE(DISCONNECTED);
	TRANSITION_CREATE(DISCONNECTED, E_SESSION_STATE_ID::SENDING_CONNECTION_CHALLENGE);

	STATE_CREATE(SENDING_CONNECTION_CHALLENGE);
	TRANSITION_CREATE(SENDING_CONNECTION_CHALLENGE, E_SESSION_STATE_ID::DISCONNECTED, E_SESSION_STATE_ID::CONNECTED);

	STATE_CREATE(CONNECTED);
	TRANSITION_CREATE(CONNECTED, E_SESSION_STATE_ID::DISCONNECTED);

#undef TRANSITIN_CREATE
#undef STATE_CREATE

	SetState(E_SESSION_STATE_ID::SENDING_CONNECTION_CHALLENGE);
}

void NetworkSession::Update(long time)
{
	if (_state != nullptr)
	{
		_state->Update(time);
	}
}

void NetworkSession::Send(ProtocolPacket& packet)
{
	_server->SendTo(packet, _address);
}

void NetworkSession::HandlePacket(ProtocolPacket* packet)
{
	assert(_state != nullptr);
	if (_state == nullptr)
		return;

	_state->HandlePacket(packet);
}

bool NetworkSession::SetState(E_SESSION_STATE_ID state_id)
{
	if (_state == nullptr)
	{
		_state = _state_map[state_id];
		_state->OnEnter();
		return true;
	}
	else
	{
		auto vect = _state_transition_table[_state->GetID()];
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

E_SESSION_STATE_ID NetworkSession::GetStateID()
{
	assert(_state != nullptr);
	if (_state == nullptr)
		return E_SESSION_STATE_ID::MAX_SESSION_STATE_ID;

	return _state->GetID();
}

bool NetworkSession::IsSameAddress(NLIBAddress& address)
{
	return _address.ip == address.ip && _address.port == address.port;
}