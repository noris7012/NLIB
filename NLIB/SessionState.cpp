#include "SessionState.h"

#include <iostream>

#include "Utility.h"
#include "ProtocolPacket.h"
#include "Logger.h"

SessionState* SessionState::Create(E_SESSION_STATE_ID state_id, NetworkSession* session)
{
	SessionState* ret;

	switch (state_id)
	{
	case E_SESSION_STATE_ID::INIT:
		ret = new SessionStateInit();
		break;
	case E_SESSION_STATE_ID::DISCONNECTED:
		ret = new SessionStateDisconnected();
		break;
	case E_SESSION_STATE_ID::SENDING_CONNECTION_CHALLENGE:
		ret = new SessionStateSendingConnectionChallenge();
		break;
	case E_SESSION_STATE_ID::CONNECTED:
		ret = new SessionStateConnected();
		break;
	default:
		return nullptr;
	}

	ret->SetSession(session);

	return ret;
}

void SessionState::SetSession(NetworkSession* session)
{
	_session = session;
}

void SessionStateInit::OnEnter()
{
	Logger::GetInstance()->Log("------------------------ Init ------------------------ ");
}

void SessionStateDisconnected::OnEnter()
{
	Logger::GetInstance()->Log("-------------------- Disconnected -------------------- ");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void SessionStateDisconnected::Update(uint64_t time)
{
	if (time >= _limit_request_time)
	{
		_session->SetState(E_SESSION_STATE_ID::INIT);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ProtocolPacketConnectionDisconnect packet;
		packet.Set(_session->GetClientID());
		_session->Send(packet);
	}
}

void SessionStateDisconnected::OnExit()
{
	_session->OnDisconnected();
}

void SessionStateDisconnected::RecvPacket(ProtocolPacket* packet)
{
	// if (packet->GetID() == E_PACKET_ID::CONNECTION_DISCONNECT)
	// {
	// 	_session->SetState(E_SESSION_STATE_ID::INIT);
	// }
}

void SessionStateSendingConnectionChallenge::OnEnter() 
{
	Logger::GetInstance()->Log("--------------- Sending Connection Challenge ---------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void SessionStateSendingConnectionChallenge::Update(uint64_t time)
{
	if (time >= _limit_request_time)
	{
		_session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

#ifdef NLIB_LOG_ENABLED
		std::cout << "[ Server Challenge Token ] " << std::endl;
		std::cout << Utility::ByteToString(_session->GetChallengeTokenEncrypted(), NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH) << std::endl;
#endif
		ProtocolPacketConnectionChallenge packet;
		packet.Set(_session->GetChallengeTokenSequence(), _session->GetChallengeTokenEncrypted());
		_session->Send(packet);
	}
}

void SessionStateSendingConnectionChallenge::RecvPacket(ProtocolPacket* p)
{
	assert(p != nullptr);
	if (p == nullptr)
		return;

	if (p->GetID() == E_PACKET_ID::CONNECTION_RESPONSE)
	{
		ProtocolPacketConnectionResponse* packet = static_cast<ProtocolPacketConnectionResponse*>(p);

		if (packet->GetChallengeTokenSequence() != _session->GetChallengeTokenSequence())
			return;

		if (memcmp(packet->GetChallengeTokenEncrypted(), _session->GetChallengeTokenEncrypted(), NLIB_CHALLENGE_TOKEN_ENCRYPTED_LENGTH) != 0)
			return;

		_session->SetState(E_SESSION_STATE_ID::CONNECTED);
	}
}

void SessionStateConnected::OnEnter()
{
	Logger::GetInstance()->Log("-------------------- Connected --------------------");
#ifdef NLIB_LOG_ENABLED
#endif // NLIB_LOG_ENABLED

	_send_keep_alive_time =	_last_recv_time = Utility::GetTime();
	_next_keep_alive_interval = 1000;

	_session->OnConnected();
}

void SessionStateConnected::Update(uint64_t time)
{
	if (time >= _last_recv_time + NLIB_CONNECT_TIMEOUT)
	{
		_session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
	}
	else if (time >= _send_keep_alive_time)
	{
		_send_keep_alive_time += _next_keep_alive_interval;

		ProtocolPacketConnectionKeepAlive packet;
		packet.Set(_session->GetClientID());
		_session->Send(packet);
	}
}

void SessionStateConnected::RecvPacket(ProtocolPacket* p)
{
	_last_recv_time = Utility::GetTime();

	if (p->GetID() == E_PACKET_ID::CONNECTION_KEEP_ALIVE)
	{
		auto packet = static_cast<ProtocolPacketConnectionKeepAlive*>(p);

		// TODO Something
	}
	else if (p->GetID() == E_PACKET_ID::CONNECTION_PAYLOAD)
	{
		auto packet = static_cast<ProtocolPacketConnectionPayload*>(p);

		_session->ReadNext(ReadParam{ packet->GetData() });
	}
	else if (p->GetID() == E_PACKET_ID::CONNECTION_DISCONNECT)
	{
		_session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
	}
}

void SessionStateConnected::Write(const WriteParam& param)
{
	auto data = param.data;

	ProtocolPacketConnectionPayload packet;
	packet.Set(_session->GetClientID());
	packet.WriteHeader(data);

	_session->Send(data);
}
