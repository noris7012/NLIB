#include "GameClient.h"

#include "GamePacket.h"
#include "GameClientHandler.h"
#include <iostream>
#include "Utility.h"

GameClient::GameClient(PGameClientHandler handler)
	: _handler(handler)
{
	_network_client = new NetworkClient(this);
	_reliable_layer = new ReliableLayer(this);

	_network_client->SetReadNext(
		std::bind(
			&ReliableLayer::Read
			, _reliable_layer
			, std::placeholders::_1
		)
	);

	_reliable_layer->SetReadNext(
		std::bind(
			&GameClient::Read
			, this
			, std::placeholders::_1
		)
	);

	SetWriteNext(
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

void GameClient::Read(ByteArrayPtr data)
{
	auto packet = GamePacket::Instance();
	packet->Set(data->Bytes(), data->Length());

	_handler->HandlePacket(shared_from_this(), packet);
}

void GameClient::WritePacket(const byte* bytes, uint32_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[WritePacket] " << Utility::TimeInHHMMSSMMM() << std::endl;
#endif

	auto data = std::make_shared<ByteArray>(const_cast<byte*>(bytes), length);

	WriteNext(data);
}
