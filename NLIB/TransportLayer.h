#ifndef NLIB_TRANSPORT_LAYER_H
#define NLIB_TRANSPORT_LAYER_H

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include <thread>

#include "NetworkDefine.h"
#include "NetworkConfig.h"
#include "NetworkEndpoint.h"
#include "NetworkStruct.h"

class NetworkEndpoint;

class TransportLayer
{
public:
	static TransportLayer* Create(E_TRANSPORT_TYPE type);

public:
	virtual void Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint) = 0;
	virtual bool IsConnected() { return false; }
	virtual void Send(S_Send data) = 0;
};

class TransportLayerUDP : public TransportLayer
{
public:
	TransportLayerUDP();
	~TransportLayerUDP();

	void Startup(NetworkConfig& config, NetworkEndpoint* local_endpoint);
	bool IsConnected();

public:
	void Send(S_Send data);
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
};

#endif