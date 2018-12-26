#ifndef NLIB_NETWORK_ENDPOINT_H
#define NLIB_NETWORK_ENDPOINT_H

#include <thread>
#include <queue>
#include <mutex>
#include <cstdint>

#include <boost/array.hpp>
#include <boost/asio.hpp>

#include "NetworkStruct.h"
#include "NetworkConfig.h"
#include "ByteStream.h"
#include "ProtocolPacket.h"
#include "BufferPool.h"

class TransportLayer;
class CUDPLayer;

class NetworkEndpoint
{
public:
	NetworkEndpoint();

	void Startup(NetworkConfig& config);
	void Destroy();
	void HandleReceive(char* data, std::size_t length, NLIBAddress& address);
	void Send(NLIBAddress& address, const byte* data, uint32_t length);
	void Send(NLIBAddress& address, UNLIBData data);

private:
	void InternalUpdate(uint64_t time);

private:
	NLIBRecv* Pop();

public:
	virtual void Update(uint64_t time) = 0;
	virtual void HandleReceive(NLIBRecv* data) = 0;

	
private:
	// Asynchronous IO
	void HandleSend(const boost::system::error_code& error, std::size_t length);

	void Receive();
	void HandleReceive(const boost::system::error_code& error, std::size_t length);

private:
	boost::asio::io_context _io_context;
	boost::asio::ip::udp::socket* _socket;
	boost::array<char, 2048> _recv_buffer;
	boost::asio::ip::udp::endpoint _remote_endpoint;
	std::thread* _io_thread;

	std::thread* _thread;
	bool _running;

	std::mutex _recv_queue_mutex;
	std::queue<NLIBRecv*> _recv_queue;

protected:
	BufferPool _buffer_pool;
};

#endif