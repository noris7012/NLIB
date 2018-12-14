#include "SessionState.h"

#include <iostream>

#include "Utility.h"
#include "ProtocolPacket.h"

SessionState* SessionState::Create(E_SESSION_STATE_ID state_id, NetworkSession* session)
{
	SessionState* ret;

	switch (state_id)
	{
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

void SessionStateDisconnected::OnEnter()
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Disconnected" << std::endl;
#endif // NLIB_LOG_ENABLED

}

void SessionStateDisconnected::Update(long time)
{

}

void SessionStateSendingConnectionChallenge::OnEnter() 
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Sending Connection Challenge" << std::endl;
#endif // NLIB_LOG_ENABLED

	_send_request_time = Utility::GetTime();
	_next_request_interval = 1000;
	_limit_request_time = _send_request_time + NLIB_CONNECT_TIMEOUT;
}

void SessionStateSendingConnectionChallenge::Update(long time) 
{
	if (time >= _limit_request_time)
	{
		_session->SetState(E_SESSION_STATE_ID::DISCONNECTED);
	}
	else if (time >= _send_request_time)
	{
		_send_request_time += _next_request_interval;

		ProtocolPacketConnectionChallenge packet;
		packet.Set(_session->GetChallengeTokenSequence(), _session->GetChallengeTokenEncrypted());
		_session->Send(packet);
	}
}

void SessionStateSendingConnectionChallenge::HandlePacket(ProtocolPacket* p)
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
#ifdef NLIB_LOG_ENABLED
	std::cout << "[OnEnter] Connected" << std::endl;
#endif // NLIB_LOG_ENABLED

	_send_keep_alive_time = Utility::GetTime();
	_next_keep_alive_interval = 1000;
}

void SessionStateConnected::Update(long time)
{
	if (time >= _send_keep_alive_time)
	{
		_send_keep_alive_time += _next_keep_alive_interval;

		ProtocolPacketConnectionKeepAlive packet;
		// TODO Set Value
		_session->Send(packet);
	}
}

void SessionStateConnected::HandlePacket(ProtocolPacket* packet)
{

}