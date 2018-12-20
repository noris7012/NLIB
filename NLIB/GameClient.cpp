#include "GameClient.h"

#include "GamePacket.h"
#include "GameClientHandler.h"

GameClient::GameClient(PGameClientHandler handler)
	: _handler(handler)
{
	_network_client = new NetworkClient();
	_reliable_layer = new ReliableLayer();

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

bool GameClient::Connect(const char* host, unsigned short port)
{
	return _network_client->Connect(host, port);
}

bool GameClient::IsConnected()
{
	return _network_client->IsConnected();
}

void GameClient::Disconnect()
{
	_network_client->Disconnect();
}

void GameClient::Read(UNLIBData data)
{
	auto packet = GamePacket::Instance();
	packet->Set(data->bytes, data->length);

	_handler->HandlePacket(shared_from_this(), packet);
}

void GameClient::Write(UNLIBData data)
{
	WriteNext(std::move(data));
}

void GameClient::WritePacket(const byte* bytes, uint32_t length)
{
	auto data = std::make_unique<NLIBData>();
	data->bytes = bytes;
	data->length = length;

	Write(std::move(data));
}
