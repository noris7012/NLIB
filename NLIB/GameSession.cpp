#include "GameSession.h"

#include "GamePacket.h"
#include "Utility.h"
#include "Logger.h"

GameSession::GameSession(GameServerHandler* handler, NetworkSession* network_session)
	: _handler(handler)
	, _network_session(network_session)
{
	_reliable_session = new ReliableLayer(this);
	_chunk_layer = new ChunkLayer(this);

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
			&ChunkLayer::Read
			, _chunk_layer
			, std::placeholders::_1
		)
	);

	_chunk_layer->SetReadNext(
		std::bind(
			&GameSession::Read
			, this
			, std::placeholders::_1
		)
	);

	//
	SetWriteNext(
		std::bind(
			&ChunkLayer::Write
			, _chunk_layer
			, std::placeholders::_1
		)
	);

	_chunk_layer->SetWriteNext(
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

	//
	_network_session->SetFailNext(
		std::bind(
			&ReliableLayer::Fail
			, _reliable_session
			, std::placeholders::_1
		)
	);

	_reliable_session->SetFailNext(
		std::bind(
			&ChunkLayer::Fail
			, _chunk_layer
			, std::placeholders::_1
		)
	);

	_chunk_layer->SetFailNext(
		std::bind(
			&GameSession::Fail
			, this
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

void GameSession::Read(ReadParam& param)
{
	auto data = param.data;
	auto offset = param.offset;

	if (data->Length() <= offset)
		return;

	int length = data->Length() - offset;

	auto new_data = new byte[length];
	memcpy(new_data, data->Bytes() + offset, length);

	GamePacket packet;
	packet.Set(new_data, length);

	auto sft = shared_from_this();
	_handler->HandlePacket(sft.get(), packet);
}

void GameSession::Write(const WriteParam& param)
{
	WriteNext(param);
}

void GameSession::Fail(const FailParam& param)
{
	auto data = param.data;

	std::stringstream ss;
	ss << "[ Fail ] " << Utility::ByteToString(data->Bytes(), data->Length());
	Logger::GetInstance()->Log(ss.str());
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
	WriteParam param {
		std::make_shared<ByteArray>(bytes, length)
	};

	Write(param);
}
