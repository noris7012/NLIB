#include "ClientState.h"

#include <iostream>
#include <assert.h>

#include "Utility.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"

ClientState* ClientState::create(E_CLIENT_STATE_ID state_id, NetworkClient* client)
{
	ClientState* ret;

	switch (state_id)
	{
	case E_CLIENT_STATE_ID::DISCONNECTED:
		ret = new ClientStateDisconnected();
		break;
	case E_CLIENT_STATE_ID::SENDING_CONNECTION_REQUEST:
		ret = new ClientStateSendingConnectionRequest();
		break;
	case E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE:
		ret = new ClientStateSendingConnectionResponse();
		break;
	case E_CLIENT_STATE_ID::CONNECTED:
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
		_client->SetState(E_CLIENT_STATE_ID::DISCONNECTED);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ProtocolPacketConnectionRequest packet;
		_client->Send(packet);
	}
}

void ClientStateSendingConnectionRequest::HandlePacket(ProtocolPacket* p)
{
	assert(p != nullptr);
	if (p == nullptr)
		return;

	if (p->GetID() == E_PACKET_ID::CONNECTION_CHALLENGE)
	{
		ProtocolPacketConnectionChallenge* packet = static_cast<ProtocolPacketConnectionChallenge*>(p);
		
		_client->SetChallengeToken(packet->GetChallengeTokenSequence(), packet->GetChallengeTokenEncrypted());
		_client->SetState(E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE);
	}
}

void ClientStateSendingConnectionResponse::OnEnter()
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Response" << std::endl;
#endif // NLIB_LOG_ENABLED

	_send_response_time = Utility::GetTime();
	_next_response_interval = 1000;
	_limit_response_time = _send_response_time + NLIB_CONNECT_TIMEOUT;
}

void ClientStateSendingConnectionResponse::Update(long time)
{
	if (time >= _limit_response_time)
	{
		_client->SetState(E_CLIENT_STATE_ID::DISCONNECTED);
	}
	else if (time >= _send_response_time)
	{
		_send_response_time += _next_response_interval;

		ProtocolPacketConnectionResponse packet;
		packet.Set(_client->GetChallengeTokenSequence(), _client->GetChallengeTokenEncrypted());
		_client->Send(packet);
	}
}

void ClientStateSendingConnectionResponse::HandlePacket(ProtocolPacket* p)
{
	assert(p != nullptr);
	if (p == nullptr)
		return;

	if (p->GetID() == E_PACKET_ID::CONNECTION_KEEP_ALIVE)
	{
		ProtocolPacketConnectionKeepAlive* packet = static_cast<ProtocolPacketConnectionKeepAlive*>(p);

		_client->SetState(E_CLIENT_STATE_ID::CONNECTED);
	}
}

void ClientStateConnected::OnEnter()
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Connected" << std::endl;
#endif // NLIB_LOG_ENABLED

	_send_keep_alive_time = Utility::GetTime();
	_next_keep_alive_interval = 1000;
}

void ClientStateConnected::Update(long time)
{
	if (time >= _send_keep_alive_time)
	{
		_send_keep_alive_time += _next_keep_alive_interval;

		ProtocolPacketConnectionKeepAlive packet;
		// TODO Set Value
		_client->Send(packet);
	}
}

void ClientStateConnected::HandlePacket(ProtocolPacket* packet)
{

}