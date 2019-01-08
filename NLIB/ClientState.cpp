#include "ClientState.h"

#include <iostream>
#include <cassert>

#include "Utility.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"
#include "Logger.h"

ClientState* ClientState::create(E_CLIENT_STATE_ID state_id, NetworkClient* client)
{
	ClientState* ret;

	switch (state_id)
	{
	case E_CLIENT_STATE_ID::INIT:
		ret = new ClientStateInit();
		break;
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

void ClientStateInit::OnEnter()
{
	Logger::GetInstance()->Log("--------------------- Init ---------------------");
}

void ClientStateDisconnected::OnEnter()
{
	Logger::GetInstance()->Log("--------------------- Disconnected ---------------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void ClientStateDisconnected::Update(uint64_t time)
{
	if (time >= _limit_request_time)
	{
		_client->SetState(E_CLIENT_STATE_ID::INIT);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ProtocolPacketConnectionDisconnect packet;
		packet.Set(_client->GetClientID());
		_client->Send(packet);
	}
}

void ClientStateDisconnected::OnExit()
{
}

void ClientStateDisconnected::RecvPacket(ProtocolPacket* packet)
{
	// if (packet->GetID() == E_PACKET_ID::CONNECTION_DISCONNECT)
	// {
	// 	_client->SetState(E_CLIENT_STATE_ID::INIT);
	// }
}

void ClientStateSendingConnectionRequest::OnEnter()
{
	Logger::GetInstance()->Log("---------------- Sending Connection Request ----------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void ClientStateSendingConnectionRequest::Update(uint64_t time)
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

void ClientStateSendingConnectionRequest::RecvPacket(ProtocolPacket* p)
{
	assert(p != nullptr);
	if (p == nullptr)
		return;

	if (p->GetID() == E_PACKET_ID::CONNECTION_CHALLENGE)
	{
		ProtocolPacketConnectionChallenge* packet = static_cast<ProtocolPacketConnectionChallenge*>(p);

#ifdef NLIB_LOG_ENABLED
		std::cout << "[ Client Challenge Token ] " << std::endl;
		std::cout << Utility::ByteToString(packet->GetChallengeTokenEncrypted(), NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH) << std::endl;
#endif
		_client->SetChallengeToken(packet->GetChallengeTokenSequence(), packet->GetChallengeTokenEncrypted());
		_client->SetState(E_CLIENT_STATE_ID::SENDING_CONNECTION_RESPONSE);
	}
}

void ClientStateSendingConnectionResponse::OnEnter()
{
	Logger::GetInstance()->Log("---------------- Sending Connection Response ----------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_response_time = Utility::GetTime();
	_next_response_interval = 1000;
	_limit_response_time = _send_response_time + NLIB_CONNECT_TIMEOUT;
}

void ClientStateSendingConnectionResponse::Update(uint64_t time)
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

void ClientStateSendingConnectionResponse::RecvPacket(ProtocolPacket* p)
{
	assert(p != nullptr);
	if (p == nullptr)
		return;

	if (p->GetID() == E_PACKET_ID::CONNECTION_KEEP_ALIVE)
	{
		ProtocolPacketConnectionKeepAlive* packet = static_cast<ProtocolPacketConnectionKeepAlive*>(p);

		_client->SetClientID(packet->GetClientID());
		_client->SetState(E_CLIENT_STATE_ID::CONNECTED);
	}
}

void ClientStateConnected::OnEnter()
{
	Logger::GetInstance()->Log("--------------------- Connected ---------------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_keep_alive_time = Utility::GetTime();
	_next_keep_alive_interval = 1000;
}

void ClientStateConnected::Update(uint64_t time)
{
	if (time >= _send_keep_alive_time)
	{
		_send_keep_alive_time += _next_keep_alive_interval;

		ProtocolPacketConnectionKeepAlive packet;
		packet.Set(_client->GetClientID());
		_client->Send(packet);
	}
}

void ClientStateConnected::RecvPacket(ProtocolPacket* p)
{
	if (p->GetID() == E_PACKET_ID::CONNECTION_PAYLOAD)
	{
		auto packet = static_cast<ProtocolPacketConnectionPayload*>(p);

		_client->ReadNext(packet->GetData());
	}
	else if (p->GetID() == E_PACKET_ID::CONNECTION_DISCONNECT)
	{
		_client->SetState(E_CLIENT_STATE_ID::DISCONNECTED);
	}
}

void ClientStateConnected::Write(ByteArrayPtr data)
{
	_send_keep_alive_time = Utility::GetTime() + _next_keep_alive_interval;

	ProtocolPacketConnectionPayload packet;
	packet.Set(_client->GetClientID());
	packet.WriteHeader(data);

	_client->Send(data);
}

