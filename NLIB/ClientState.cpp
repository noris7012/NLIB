#include "ClientState.h"

#include "Utility.h"
#include "ByteStream.h"

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

void ClientStateDisconnected::Update(long time)
{

}

void ClientStateSendingConnectionRequest::OnEnter()
{
	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
}

void ClientStateSendingConnectionRequest::Update(long time)
{
	if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ByteStream stream;
		stream.Write(E_PACKET_ID::CONNECTION_REQUEST);
		stream.Write(NLIB_PROTOCOL_ID);
		stream.PadWithZero();

		_client->Send(stream);
	}
}