#include "TransportLayer.h"

#include <boost/bind.hpp>
#include "NetworkStruct.h"
#include <stdlib.h>
#include <iostream>

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
{

}

TransportLayerUDP::~TransportLayerUDP()
{

}

void TransportLayerUDP::Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint)
{
	_local_endpoint = local_endpoint;

	_thread = new std::thread([this, config]()
	{
		boost::asio::io_context io_context;

		if (config.port > 0)
		{
			udp::resolver resolver(io_context);
			char s_port[10];
			snprintf(s_port, 10, "%d", config.port);
			udp::resolver::query query(udp::v4(), config.host, s_port);
			_remote_endpoint = *resolver.resolve(query).begin();

			_socket = new udp::socket(io_context, udp::endpoint(udp::v4(), config.port));

			Receive();
		}
		else
		{
			_socket = new udp::socket(io_context, udp::v4());

			Receive();
		}

		io_context.run();
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
		return;
	}

	S_Recv_Ptr recv = std::make_shared<S_Recv>();
	assert(sizeof(recv->data) == MAX_MTU_SIZE);
	memcpy_s(recv->data, sizeof(recv->data), _recv_buffer.data(), length);
	recv->length = length;

	_local_endpoint->HandleReceive(recv);

	Receive();
}

void TransportLayerUDP::Send(S_Send data)
{
	_socket->async_send_to(
		boost::asio::buffer(data.data)
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
	if (error)
	{
		std::cout << error.message() << std::endl;
	}
}