#include "NetworkEndpoint.h"

#include <boost/bind.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "Utility.h"
#include "Logger.h"

using namespace boost::asio::ip;

NetworkEndpoint::NetworkEndpoint()
	: _thread(nullptr)
	, _running(false)
{
}

void NetworkEndpoint::Startup(GameConfig& config)
{
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

	_io_thread = new std::thread([this]() {
		_io_context.run();
	});

	_thread = new std::thread([this]()
	{
		using namespace std::chrono;

		_running = true;
		while (_running)
		{
			auto time = Utility::GetTime();

			InternalUpdate(time);
			Update(time);

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	{
		// Test
		_loss_mask = config.loss_mask;
	}
}

void NetworkEndpoint::Destroy()
{
	/*
	_running = false;
	_thread->join();

	delete _thread;
	_thread = nullptr;

	_io_context.stop();
	_thread->join();
	delete _thread;
	_thread = nullptr;

	delete _socket;
	// _local_endpoint 가 더 상위 객체이므로 여기서 delete를 하진 않는다.
	_local_endpoint = nullptr;
	delete _transport;
	_transport = nullptr;
	*/
}

void NetworkEndpoint::InternalUpdate(uint64_t time)
{
	while (true)
	{
		// TODO Implement Synchronized Queue
		_recv_queue_mutex.lock();
		if (_recv_queue.empty())
		{
			_recv_queue_mutex.unlock();
			break;			
		}

		auto data = std::move(_recv_queue.front());
		_recv_queue.pop();
		_recv_queue_mutex.unlock();

		HandleReceive(std::move(data));
	}
}

void NetworkEndpoint::HandleReceive(byte* data, std::size_t length, NLIBAddress& address)
{
	// TODO Get ByteArrayPtr from Pool
	auto new_data = std::make_shared<ByteArray>(length);
	new_data->Set(NLIB_OFFSET_NETWORK, data, length);

	NLIBRecv recv;
	recv.data = new_data;
	recv.address = address;

	// TODO Implement Synchronized Queue
	_recv_queue_mutex.lock();
	_recv_queue.push(std::move(recv));
	_recv_queue_mutex.unlock();
}

void NetworkEndpoint::Send(NLIBAddress& address, const byte* data, uint32_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[ Send ] " << Utility::ByteToString(data, length) << std::endl;
#endif

	_socket->async_send_to(
		boost::asio::buffer(data, length)
		, udp::endpoint(address::from_string(address.ip_str), address.port)
		, boost::bind(&NetworkEndpoint::HandleSend
			, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

void NetworkEndpoint::Send(NLIBAddress& address, ByteArrayPtr data)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[ Send ] " << Utility::ByteToString(data->Bytes(), data->Length()) << std::endl;
#endif
	_send_buffer = data;

	using namespace boost::asio;

	_socket->async_send_to(
		buffer(_send_buffer->Bytes(), _send_buffer->Length())
		, udp::endpoint(address::from_string(address.ip_str), address.port)
		, boost::bind(&NetworkEndpoint::HandleSend
			, this,
			placeholders::error,
			placeholders::bytes_transferred
		)
	);
}

void NetworkEndpoint::HandleSend(const boost::system::error_code& error, std::size_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[HandleSend] " << Utility::TimeInHHMMSSMMM() << std::endl;
#endif

	if (error)
	{
		std::cout << error.message() << std::endl;
	}
	assert(!error);
}

void NetworkEndpoint::Receive()
{
	_socket->async_receive_from(
		boost::asio::buffer(_recv_buffer)
		, _remote_endpoint
		, boost::bind(
			&NetworkEndpoint::HandleReceive
			, this
			, boost::asio::placeholders::error
			, boost::asio::placeholders::bytes_transferred
		)
	);
}

void NetworkEndpoint::HandleReceive(const boost::system::error_code& error, std::size_t length)
{
#ifdef NLIB_LOG_ENABLED
	std::cout << "[HandleRecv] " << Utility::TimeInHHMMSSMMM() << std::endl;
#endif

	assert(!error);
	if (error)
	{
		std::cout << error.message() << std::endl;
		return;
	}

	if (_loss_mask != nullptr && !_loss_mask->at(_loss_index = (_loss_index + 1) % _loss_mask->size()))
	{
		std::stringstream stream;
		stream << "[Loss] " << Utility::ByteToString(reinterpret_cast<const byte*>(_recv_buffer.data()), std::min(length, size_t(30)));
		Logger::GetInstance()->Log(stream.str());
	}
	else
	{
		std::stringstream stream;
		stream << "[Recv] " << Utility::ByteToString(reinterpret_cast<const byte*>(_recv_buffer.data()), std::min(length, size_t(30)));
		Logger::GetInstance()->Log(stream.str());

		NLIBAddress address;
		address.ip_str = _remote_endpoint.address().to_v4().to_string();
		address.ip = _remote_endpoint.address().to_v4().to_ulong();
		address.port = _remote_endpoint.port();

		HandleReceive(_recv_buffer.data(), length, address);		
	}

	Receive();
}