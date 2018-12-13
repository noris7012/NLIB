#include "TransportLayer.h"

#include <boost/bind.hpp>
#include <stdlib.h>
#include <iostream>

#include "NetworkStruct.h"

using namespace boost::asio::ip;

TransportLayer* TransportLayer::Create(E_TRANSPORT_TYPE type)
{
	switch (type)
	{
	case UDP:
		return new TransportLayerUDP();
	default:
		return nullptr;
	}
}

TransportLayerUDP::TransportLayerUDP()
	: _remote_endpoint()
{

}

TransportLayerUDP::~TransportLayerUDP()
{

}

void TransportLayerUDP::Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint)
{
	_local_endpoint = local_endpoint;

	//boost::asio::io_context io_context;

	if (config.host != nullptr)
	{
		// TODO Á» ´õ ´Ùµë±â
		udp::resolver resolver(_io_context);
		char s_port[10];
		snprintf(s_port, 10, "%d", config.port);
		udp::resolver::query query(udp::v4(), config.host, s_port);
		_remote_endpoint = *resolver.resolve(query).begin();

		_socket = new udp::socket(_io_context, udp::endpoint(udp::v4(), 0));

		Receive();
	}
	else
	{
		_socket = new udp::socket(_io_context, udp::endpoint(udp::v4(), config.port));

		Receive();
	}

	_thread = new std::thread([this]() {
		_io_context.run();
	});
}

bool TransportLayerUDP::IsConnected()
{
	return _socket != nullptr;
}

void TransportLayerUDP::Receive()
{
	_socket->async_receive_from(
		boost::asio::buffer(_recv_buffer)
		, _remote_endpoint
		, boost::bind(
			&TransportLayerUDP::HandleReceive
			, this
			, boost::asio::placeholders::error
			, boost::asio::placeholders::bytes_transferred
		)
	);
}

void TransportLayerUDP::HandleReceive(const boost::system::error_code& error, std::size_t length)
{
	assert(!error);
	if (error)
	{
		std::cout << error.message() << std::endl;
		return;
	}

	NLIBAddress address;
	address.ip = _remote_endpoint.address().to_v4().to_ulong();
	address.port = _remote_endpoint.port();

	_local_endpoint->HandleReceive(_recv_buffer.data(), length, address);

	Receive();
}

void TransportLayerUDP::Send(ByteStream& stream)
{
	_socket->async_send_to(
		boost::asio::buffer(stream.Data(), stream.Length())
		, _remote_endpoint
		, boost::bind(&TransportLayerUDP::HandleSend
			, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void TransportLayerUDP::HandleSend(const boost::system::error_code& error, std::size_t length)
{
	assert(!error);
	if (error)
	{
		std::cout << error.message() << std::endl;
	}
}