#include "GameSession.h"

#include "GamePacket.h"

GameSession::GameSession(PGameServerHandler handler, NetworkSession* network_session)
	: _handler(handler)
	, _network_session(network_session)
{
	_reliable_session = new ReliableLayer(this);

	//
	_network_session->SetReadNext(
		std::bind(
			&ReliableLayer::Read
			, _reliable_session
			, std::placeholders::_1
		)
	);

	_reliable_session->SetReadNext(
		std::bind(
			&GameSession::Read
			, this
			, std::placeholders::_1
		)
	);

	SetWriteNext(
		std::bind(
			&ReliableLayer::Write
			, _reliable_session
			, std::placeholders::_1
		)
	);

	_reliable_session->SetWriteNext(
		std::bind(
			&NetworkSession::Write
			, _network_session
			, std::placeholders::_1
		)
	);
}

GameSession::~GameSession()
{
	delete _reliable_session;
	delete _network_session;
}

void GameSession::RecvPacket(ProtocolPacket* recv)
{
	_network_session->RecvPacket(recv);
}

void GameSession::Read(PNLIBData data)
{
	auto packet = GamePacket::Instance();
	packet->Set(data->bytes, data->length);

	_handler->HandlePacket(shared_from_this(), packet);
}

void GameSession::Write(PNLIBData data)
{
	WriteNext(data);
}

void GameSession::Update(uint64_t time)
{
	_network_session->Update(time);

	// TODO 여기서 처리하는게 맞을지..
	if (IsConnected())
		_reliable_session->Update(time);
}

bool GameSession::IsConnected()
{
	return _network_session->IsConnected();
}

void GameSession::Close()
{
	_network_session->Close();
}

void GameSession::WritePacket(const byte* bytes, uint32_t length)
{
	auto data = NLIBData::Instance();
	data->bytes = bytes;
	data->length = length;

	Write(data);
}
