#include "NetworkClient.h"

NetworkClient::NetworkClient()
	: _state(NULL), _state_map(), _state_transition_table()
{
#define STATE_CREATE(id) ( _state_map[id] = ClientState::create(id) )
#define TRANSITION_CREATE(id, ...) ( _state_transition_table[id] = new std::vector<E_CLIENT_STATE_ID>{ ##__VA_ARGS__ } )

	STATE_CREATE(DISCONNECTED);
	TRANSITION_CREATE(DISCONNECTED, SENDING_CONNECTIN_REQUEST);

	STATE_CREATE(SENDING_CONNECTIN_REQUEST);
	TRANSITION_CREATE(SENDING_CONNECTIN_REQUEST, DISCONNECTED, SENDING_CONNECTIN_RESPONSE);

	STATE_CREATE(SENDING_CONNECTIN_RESPONSE);
	TRANSITION_CREATE(SENDING_CONNECTIN_RESPONSE, DISCONNECTED, CONNECTED);

#undef TRANSITIN_CREATE
#undef STATE_CREATE

	SetState(DISCONNECTED);
}

NetworkClient::~NetworkClient()
{

}

bool NetworkClient::connect(const char* host, unsigned short port)
{
	// TODO Check State
	NetworkConfig config;
	config.transport_type = UDP;
	config.host = host;
	config.port = port;

	Startup(config);

	return SetState(SENDING_CONNECTIN_REQUEST);
}

void NetworkClient::Update(long time)
{
	if (_state != NULL)
	{
		_state->Update(time);
	}
}

bool NetworkClient::SetState(E_CLIENT_STATE_ID state_id)
{
	if (_state == NULL)
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
