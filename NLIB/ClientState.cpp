#include "ClientState.h"

ClientState* ClientState::create(E_CLIENT_STATE_ID state_id)
{
	switch (state_id)
	{
	case DISCONNECTED:
		return new ClientStateDisconnected();
	case SENDING_CONNECTIN_REQUEST:
		return new ClientStateSendingConnectionRequest();
	case SENDING_CONNECTIN_RESPONSE:
		return new ClientStateSendingConnectionResponse();
	case CONNECTED:
		return new ClientStateConnected();
	default:
		return nullptr;
	}
}

void ClientStateDisconnected::Update(long time)
{

}

