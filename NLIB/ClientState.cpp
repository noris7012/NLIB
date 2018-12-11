#include "ClientState.h"

#include <iostream>

#include "Utility.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"

ClientState* ClientState::create(E_CLIENT_STATE_ID state_id, NetworkClient* client)
{
	ClientState* ret;

	switch (state_id)
	{
	case DISCONNECTED:
		ret = new ClientStateDisconnected();
		break;
	case SENDING_CONNECTIN_REQUEST:
		ret = new ClientStateSendingConnectionRequest();
		break;
	case SENDING_CONNECTIN_RESPONSE:
		ret = new ClientStateSendingConnectionResponse();
		break;
	case CONNECTED:
		ret = new ClientStateConnected();
		break;
	default:
		return nullptr;
	}

	ret->SetClient(client);

	return ret;
}

void ClientState::SetClient(NetworkClient* client)
{
	_client = client;
}

void ClientStateDisconnected::OnEnter()
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Disconnected" << std::endl;
#endif // NLIB_LOG_ENABLED

}

void ClientStateDisconnected::Update(long time)
{

}

void ClientStateSendingConnectionRequest::OnEnter()
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Sending Connection Request" << std::endl;
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void ClientStateSendingConnectionRequest::Update(long time)
{
	if (time >= _limit_request_time)
	{
		_client->SetState(DISCONNECTED);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ProtocolPacketConnectionRequest packet;
		_client->Send(packet);
	}
}