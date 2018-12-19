#include "TransportLayer.h"

#include <boost/bind.hpp>
#include <stdlib.h>
#include <iostream>

#include "NetworkStruct.h"
#include "Utility.h"

using namespace boost::asio::ip;

void TransportLayer::Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint)
{
	_local_endpoint = local_endpoint;

	//boost::asio::io_context io_context;

	if (config.host != nullptr)
	{
		// TODO 좀 더 다듬기
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

void TransportLayer::Destroy()
{
	_io_context.stop();
	_thread->join();
	delete _thread;
	_thread = nullptr;

	delete _socket;
	// _local_endpoint 가 더 상위 객체이므로 여기서 delete를 하진 않는다.
	_local_endpoint = nullptr;
}

bool TransportLayer::IsConnected()
{
	return _socket != nullptr;
}

void TransportLayer::Receive()
{
	_socket->async_receive_from(
		boost::asio::buffer(_recv_buffer)
		, _remote_endpoint
		, boost::bind(
			&TransportLayer::HandleReceive
			, this
			, boost::asio::placeholders::error
			, boost::asio::placeholders::bytes_transferred
		)
	);
}

void TransportLayer::HandleReceive(const boost::system::error_code& error, std::size_t length)
{
	assert(!error);
	if (error)
	{
		std::cout << error.message() << std::endl;
		return;
	}

	NLIBAddress address;
	address.ip_str = _remote_endpoint.address().to_v4().to_string();
	address.ip = _remote_endpoint.address().to_v4().to_ulong();
	address.port = _remote_endpoint.port();

	_local_endpoint->HandleReceive(_recv_buffer.data(), length, address);

	Receive();
}

void TransportLayer::Send(NLIBAddress& address, const byte* data, uint32_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[ Send ] " << std::endl;
	std::cout << Utility::ByteToString(data, length) << std::endl;
#endif

	_socket->async_send_to(
		boost::asio::buffer(data, length)
		, udp::endpoint(address::from_string(address.ip_str), address.port)
		, boost::bind(&TransportLayer::HandleSend
			, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void TransportLayer::HandleSend(const boost::system::error_code& error, std::size_t length)
{
	assert(!error);
	if (error)
	{
		std::cout << error.message() << std::endl;
	}
}