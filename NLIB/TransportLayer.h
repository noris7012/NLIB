#ifndef NLIB_TRANSPORT_LAYER_H
#define NLIB_TRANSPORT_LAYER_H

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <thread>

#include "NetworkDefine.h"
#include "NetworkConfig.h"
#include "NetworkEndpoint.h"
#include "NetworkStruct.h"
#include "ByteStream.h"

class NetworkEndpoint;

class TransportLayer
{
public:
	void Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint);
	void Destroy();
	bool IsConnected();

public:
	void Send(NLIBAddress& address, const byte* data, uint32_t length);
	void Send(NLIBAddress& address, UNLIBData data);

private:
	void HandleSend(const boost::system::error_code& error, std::size_t length);

	void Receive();
	void HandleReceive(const boost::system::error_code& error, std::size_t length);

private:
	std::thread* _thread;
	boost::asio::ip::udp::socket* _socket;
	boost::array<char, 2048> _recv_buffer;
	boost::asio::ip::udp::endpoint _remote_endpoint;

	NetworkEndpoint* _local_endpoint;

	boost::asio::io_context _io_context;
};

#endif