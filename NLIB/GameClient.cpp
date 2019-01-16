#include "GameClient.h"

#include "GamePacket.h"
#include "GameClientHandler.h"
#include <iostream>
#include "Utility.h"
#include "Logger.h"

GameClient::GameClient(GameClientHandler* handler)
	: _handler(handler)
{
	_network_client = new NetworkClient(this);
	_reliable_layer = new ReliableLayer(this);
	_chunk_layer = new ChunkLayer(this);

	_network_client->SetReadNext(
		std::bind(
			&ReliableLayer::Read
			, _reliable_layer
			, std::placeholders::_1
		)
	);

	_reliable_layer->SetReadNext(
		std::bind(
			&ChunkLayer::Read
			, _chunk_layer
			, std::placeholders::_1
		)
	);

	_chunk_layer->SetReadNext(
		std::bind(
			&GameClient::Read
			, this
			, std::placeholders::_1
		)
	);

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
			, _reliable_layer
			, std::placeholders::_1
		)
	);

	_reliable_layer->SetWriteNext(
		std::bind(
			&NetworkClient::Write
			, _network_client
			, std::placeholders::_1
		)
	);

	_network_client->SetFailNext(
		std::bind(
			&ReliableLayer::Fail
			, _reliable_layer
			, std::placeholders::_1
		)
	);

	_reliable_layer->SetFailNext(
		std::bind(
			&ChunkLayer::Fail
			, _chunk_layer
			, std::placeholders::_1
		)
	);

	_chunk_layer->SetFailNext(
		std::bind(
			&GameClient::Fail
			, this
			, std::placeholders::_1
		)
	);
}

bool GameClient::Connect(GameConfig& config)
{
	return _network_client->Connect(config);
}

bool GameClient::IsConnected()
{
	return _network_client->IsConnected();
}

void GameClient::Disconnect()
{
	_network_client->Disconnect();
}


void GameClient::Update(uint64_t time)
{
	// Network Client 의 Thread 가 호출해주므로 _network_client->Update() 를 호출하면 무한 재귀

	// TODO 여기서 처리하는게 맞을지..
	if (IsConnected())
		_reliable_layer->Update(time);
}

void GameClient::Read(ReadParam& param)
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

	_handler->HandlePacket(this, packet);
}

void GameClient::WritePacket(const byte* bytes, uint32_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[WritePacket] " << Utility::TimeInHHMMSSMMM() << std::endl;
#endif

	WriteParam param {
		std::make_shared<ByteArray>(const_cast<byte*>(bytes), length)
	};

	WriteNext(param);
}

void GameClient::Fail(const FailParam& param)
{
	auto data = param.data;

	std::stringstream ss;
	ss << "[ Fail ] " << Utility::ByteToString(data->Bytes(), data->Length());
	Logger::GetInstance()->Log(ss.str());
}
