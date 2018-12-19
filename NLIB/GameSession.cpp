#include "GameSession.h"

GameSession::GameSession(PGameServerHandler handler, NetworkSession* network_session)
	: _handler(handler)
	, _network_session(network_session)
{
	_reliable_session = new ReliableSession();

	// TODO Refactoring
	_network_session->_recv_next = std::bind(&ReliableSession::OnRecv, _reliable_session, std::placeholders::_1);
	_reliable_session->_recv_next = std::bind(&GameSession::OnRecvT, this, std::placeholders::_1);
}

GameSession::~GameSession()
{
	delete _reliable_session;
	delete _network_session;
}

void GameSession::OnRecv(ProtocolPacket* recv)
{
	_network_session->OnRecv(recv);
}

void GameSession::OnRecvT(NLIBData data)
{
	auto packet = std::make_shared<GamePacket>();

	_handler->HandleReceive(shared_from_this(), packet);
}

void GameSession::OnSend(NLIBData data)
{
	auto buffer = _buffer_pool.Acquire();

	// TODO Append Header with out memcpy


	_buffer_pool.Release(buffer);
}